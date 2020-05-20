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
        case TOTP:
            this->_counter = 0; // unused for TOTP
            break;
        case HOTP:
            this->_period = 0; // unused for HOTP
            break;
        case Steam:
            this->_digits = 5; // Steam tokens are 5 digits long
            this->_algorithm = SHA1; // force algorithm to SHA1 for Steam
            break;
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
    // overwrite after default initialization
    this->_digits = digits;
    this->_period = period;
    this->_counter = counter;
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
        // TODO: report error
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
        static const std::string steam_alphabet = "23456789BCDFGHJKMNPQRTVWXY";

        const auto timestamp = time / 30;

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

        char code[6];
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
