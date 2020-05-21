#include "otptoken.hpp"
#include "private/serialize.hpp"
#include "private/otpgen.hpp"

#include <sstream>

const std::uint32_t OTPToken::VERSION = 0x00000001;

namespace
{
    // token type names
    const std::string str_unknown = "(unknown)";
    const std::string str_totp    = "TOTP";
    const std::string str_hotp    = "HOTP";
    const std::string str_steam   = "Steam";

    // token defaults config
    struct defaults
    {
        std::uint8_t digits;
        std::uint32_t period;
        std::uint32_t counter;
        OTPToken::Algorithm algorithm;
    };

    // token defaults
    const auto totp_defaults =  defaults{6, 30,  0, OTPToken::SHA1};
    const auto hotp_defaults =  defaults{6,  0,  0, OTPToken::SHA1};
    const auto steam_defaults = defaults{5, 30,  0, OTPToken::SHA1};

    // steam token alphabet
    static const std::string steam_alphabet = "23456789BCDFGHJKMNPQRTVWXY";

    // converts Steam's base64 secrets into a compatible base32 string for the OTP generator
    static const std::string convert_steam_base64_secret(const std::string &steam_base64_secret)
    {
        // input can't be empty
        if (steam_base64_secret.empty())
        {
            return {};
        }

        std::string base32;

        try {

            // create an RFC 4648 base-32 encoder
            // crypto++ uses DUDE by default which isn't TOTP compatible
            auto encoder = new CryptoPP::Base32Encoder();
            static const CryptoPP::byte ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
            static const CryptoPP::AlgorithmParameters params = CryptoPP::MakeParameters(
                                                                CryptoPP::Name::EncodingLookupArray(),
                                                                static_cast<const CryptoPP::byte*>(ALPHABET));
            encoder->IsolatedInitialize(params);

            encoder->Attach(new CryptoPP::StringSink(base32));

            // decode and re-encode base-64 data into base-32
            CryptoPP::StringSource src(steam_base64_secret, true,
                new CryptoPP::Base64Decoder(encoder));

        } catch (...) {
            return {};
        }

        return base32;
    }
}

OTPToken::OTPToken(
        const std::string &label,
        const std::string &secret,
        Type type,
        Algorithm algorithm)
    : _label(label),
      _secret(secret),
      _type(type),
      _algorithm(algorithm)
{
    switch (this->_type)
    {
        case TOTP:  this->set_defaults(&totp_defaults);  break;
        case HOTP:  this->set_defaults(&hotp_defaults);  break;
        case Steam: this->set_defaults(&steam_defaults); break;
    }

    if (this->_type != Steam)
    {
        this->_algorithm = algorithm;
    }
}

OTPToken::OTPToken(const std::string &label,
        const std::string &secret,
        const std::uint8_t &digits,
        const std::uint32_t &period,
        const std::uint32_t &counter,
        Type type,
        Algorithm algorithm)
    : OTPToken(label, secret, type, algorithm)
{
    // overwrite values after default initialization, if applicable

    // don't change anything when OTP type is Steam
    if (this->_type == Steam)
    {
        return;
    }

    // every token type can have a custom digit length
    this->_digits = digits;

    // only non-HOTP tokens can have a period
    if (this->_type != HOTP)
    {
        this->_period = period;
    }

    // only HOTP tokens can have a counter
    if (this->_type == HOTP)
    {
        this->_counter = counter;
    }
}

OTPToken::OTPToken(const Data &data)
{
    // create a string stream for cereal
    const std::string strdata(data.data(), data.size());
    std::istringstream buffer(strdata);

    OTPToken self;
    cereal::PortableBinaryInputArchive archive(buffer);
    try {
        archive(self);
    }  catch (cereal::Exception &e) {
        this->valid = false;
    }

    *this = self;
}

OTPToken::~OTPToken()
{
}

const std::string &OTPToken::typeName() const
{
    switch (this->_type)
    {
        case TOTP:  return str_totp;
        case HOTP:  return str_hotp;
        case Steam: return str_steam;
        default:    return str_unknown;
    }
}

const OTPToken::Data OTPToken::serialize() const
{
    std::ostringstream buffer;
    cereal::PortableBinaryOutputArchive archive(buffer);
    archive(*this);
    const auto str = buffer.str();
    return Data(str.data(), str.data() + str.size());
}

const std::string OTPToken::generate(Error *error) const
{
    return this->generate(std::time(nullptr), error);
}

const std::string OTPToken::generate(const std::time_t &time, Error *error) const
{
    if (!check_otp_length(this->_digits))
    {
        if (error)
        {
            (*error) = InvalidDigits;
        }
        return {};
    }

    if (this->_type == TOTP)
    {
        if (!check_period(this->_period))
        {
            if (error)
            {
                (*error) = InvalidPeriod;
            }
            return {};
        }

        const auto timestamp = time / this->_period;

        // use hotp with the timestamp as counter to compute a totp token
        return hotp_helper(this->_secret, timestamp, this->_digits, this->_algorithm, error);
    }

    else if (this->_type == HOTP)
    {
        return hotp_helper(this->_secret, this->_counter, this->_digits, this->_algorithm, error);
    }

    else if (this->_type == Steam)
    {
        const auto timestamp = time / 30; // hardcode 30 seconds besides default handling

        const auto hmac = compute_hmac(this->_secret, timestamp, OTPToken::SHA1);
        if (hmac.empty())
        {
            if (error)
            {
                (*error) = InvalidBase32Input;
            }
            return {};
        }

        unsigned long offset = (hmac[SHA1_DIGEST_SIZE-1] & 0x0f);
        auto bin_code = compute_bin_code(hmac, offset);

        char code[6]; // hardcode digit length besides default handing
        for (auto i = 0; i < 5; ++i)
        {
            int mod = bin_code % steam_alphabet.size();
            bin_code = bin_code / steam_alphabet.size();
            code[i] = steam_alphabet[mod];
        }
        code[5] = '\0';

        return std::string(code, code + strlen(code));
    }

    else
    {
        return {};
    }
}

void OTPToken::set_defaults(const void *_def)
{
    const auto *def = static_cast<const defaults*>(_def);
    this->_digits = def->digits;
    this->_period = def->period;
    this->_counter = def->counter;
    this->_algorithm = def->algorithm;
}
