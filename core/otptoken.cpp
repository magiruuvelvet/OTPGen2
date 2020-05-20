#include "otptoken.hpp"
#include "private/serialize.hpp"

#include <sstream>

#include <cryptopp/filters.h>
#include <cryptopp/base32.h>
#include <cryptopp/base64.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>

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
        Type type,
        Algorithm algorithm)
    : OTPToken(label, secret, type, algorithm)
{
    // overwrite after default initialization
    this->_digits = digits;
    this->_period = period;
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
