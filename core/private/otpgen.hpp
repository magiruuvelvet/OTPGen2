#ifndef CORE_PRIVATE_OTPGEN_HPP
#define CORE_PRIVATE_OTPGEN_HPP

#include <cryptopp/filters.h>
#include <cryptopp/base32.h>
#include <cryptopp/base64.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>

#include <otptoken.hpp>

#include <cstdlib>
#include <cstdint>
#include <ctime>

namespace
{

static const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static const constexpr auto SHA1_DIGEST_SIZE = 20;
static const constexpr auto SHA256_DIGEST_SIZE = 32;
static const constexpr auto SHA512_DIGEST_SIZE = 64;

static const constexpr std::uint64_t DIGITS_POWER[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
};

static const std::string normalize_secret(const std::string &secret)
{
    auto nK = static_cast<char*>(std::calloc(1, secret.size() + 1));

    std::size_t i = 0, j = 0;
    while (secret[i] != '\0')
    {
        if (secret[i] != ' ')
        {
            if (secret[i] >= 'a' && secret[i] <= 'z')
            {
                nK[j++] = static_cast<char>( (secret[i] - 32) );
            }
            else
            {
                nK[j++] = secret[i];
            }
        }
        i++;
    }

    std::string normalized(nK, nK + strlen(nK));
    std::free(nK);
    return normalized;
}

static const std::string base32_rfc4648_decode(const std::string &key)
{
    if (key.empty())
    {
        return {};
    }

    // create an RFC 4648 base-32 decoder
    // crypto++ uses DUDE by default which isn't TOTP compatible
    auto decoder = new CryptoPP::Base32Decoder();

    static int lookup[256];

    static const auto init = ([&]{
        CryptoPP::Base64Decoder::InitializeDecodingLookupArray(lookup, ALPHABET, 32, true);
        return 0;
    })();
    (void) init;

    static const CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(
                                                        CryptoPP::Name::DecodingLookupArray(),
                                                        static_cast<const int*>(lookup));
    decoder->IsolatedInitialize(params);

    // raw pointers are automatically deleted by crypto++
    std::string base32;
    decoder->Attach(new CryptoPP::StringSink(base32));

    // result may be binary (unsigned char)
    try {
        CryptoPP::StringSource(key, true, decoder);
    } catch (...) {
        return {};
    }

    return base32;
}

// template helper function to compute HMAC's of different SHA algorithms
template<class CryptoPPHMacClass>
static inline const std::string compute_hmac_helper(const std::string &key, unsigned char value[8])
{
    CryptoPPHMacClass cryptoHmac(reinterpret_cast<const unsigned char*>(key.data()), key.size());

    std::string hmac;
    CryptoPP::StringSource(value, 8, true,
        new CryptoPP::HashFilter(cryptoHmac,
            new CryptoPP::StringSink(hmac)));

    return hmac;
}

static const std::string compute_hmac(const std::string &key, long C, const OTPToken::Algorithm &algo)
{
    // normalize and decode secret
    const auto normalized_key = normalize_secret(key);
    const auto secret = base32_rfc4648_decode(normalized_key);

    // don't continue on empty secret
    if (secret.empty())
    {
        return {};
    }

    // calculate reverse byte order of C
    unsigned char C_reverse_byte_order[8];
    int j, i;
    for (j = 0, i = 7; j < 8 && i >= 0; ++j, --i)
    {
        C_reverse_byte_order[i] = (reinterpret_cast<unsigned char*>(&C))[j];
    }

    // compute the HMAC and store it into a string
    std::string hmac;
    switch (algo)
    {
        case OTPToken::SHA1:   hmac = compute_hmac_helper<CryptoPP::HMAC<CryptoPP::SHA1>>(secret, C_reverse_byte_order); break;
        case OTPToken::SHA256: hmac = compute_hmac_helper<CryptoPP::HMAC<CryptoPP::SHA256>>(secret, C_reverse_byte_order); break;
        case OTPToken::SHA512: hmac = compute_hmac_helper<CryptoPP::HMAC<CryptoPP::SHA512>>(secret, C_reverse_byte_order); break;
    }

    // validate HMAC
    if (hmac.empty())
    {
        return {};
    }

    return hmac;
}

static const std::string finalize(const std::uint8_t &digit_length, int tk)
{
    auto token = static_cast<char*>(std::malloc(digit_length + 1));

    int extra_char = digit_length < 10 ? 0 : 1;
    auto fmt = static_cast<char*>(std::calloc(1, 5 + extra_char));
    std::memcpy(fmt, "%.", 2);
    std::snprintf(fmt + 2, 2 + extra_char, "%d", digit_length);
    std::memcpy(fmt + 3 + extra_char, "d", 2);
    std::snprintf(token, digit_length + 1, fmt, tk);
    std::free(fmt);

    std::string tokenStr(token, token + strlen(token));
    std::free(token);
    return tokenStr;
}

static int compute_bin_code(const std::string &hmac, unsigned long offset)
{
    // starting from the offset, take the successive 4 bytes while stripping
    // the topmost bit to prevent it being handled as a signed integer
    return
        ((hmac[offset] & 0x7f) << 24) |
        ((hmac[offset + 1] & 0xff) << 16) |
        ((hmac[offset + 2] & 0xff) << 8) |
        ((hmac[offset + 3] & 0xff));
}

static int truncate(const std::string &hmac, const std::uint8_t &digit_length, const OTPToken::Algorithm algo)
{
    // take the lower four bits of the last byte
    unsigned long offset = 0;
    switch (algo)
    {
        case OTPToken::SHA1:
            offset = (hmac[SHA1_DIGEST_SIZE-1] & 0x0f);
            break;
        case OTPToken::SHA256:
            offset = (hmac[SHA256_DIGEST_SIZE-1] & 0x0f);
            break;
        case OTPToken::SHA512:
            offset = (hmac[SHA512_DIGEST_SIZE-1] & 0x0f);
            break;
    }

    auto bin_code = compute_bin_code(hmac, offset);
    int token = bin_code % DIGITS_POWER[digit_length];
    return token;
}

static const std::string hotp_helper(const std::string &base32_secret,
                                     const std::time_t &counter,
                                     const std::uint8_t &digits,
                                     const OTPToken::Algorithm &algo,
                                     OTPToken::Error *error = nullptr)
{
    const auto hmac = compute_hmac(base32_secret, counter, algo);
    if (hmac.empty())
    {
        if (error)
        {
            (*error) = OTPToken::InvalidBase32Input;
        }
        return {};
    }

    auto tk = truncate(hmac, digits, algo);
    return finalize(digits, tk);
}

static bool check_period(const std::uint32_t &period)
{
    return !(period <= 0 || period > 120);
}

static bool check_otp_length(const std::uint8_t &digit_length)
{
    return !(digit_length < 1 || digit_length > 10);
}

} // anonymous namespace

#endif // CORE_PRIVATE_OTPGEN_HPP
