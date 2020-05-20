#ifndef OTPTOKEN_HPP
#define OTPTOKEN_HPP

#include <string>
#include <vector>
#include <cstdint>

class OTPToken
{
public:

    using Data = std::vector<char>;

    /**
     * error codes
     */
    enum Error
    {
        Valid = 0,
        InvalidBase32Input,
        InvalidAlgorithm,
        InvalidOTP,
        InvalidDigits,
        InvalidPeriod,
    };

    /**
     * token types
     */
    enum Type
    {
        TOTP = 1,
        HOTP = 2,
        Steam = 3,
    };

    /**
     * token algorithm
     */
    enum Algorithm
    {
        SHA1    = 1,
        SHA256  = 2,
        SHA512  = 3,
    };

public:
    /**
     * Constructs a new OTPToken with default values.
     *
     * By default a TOTP token with SHA-1 is created.
     */
    OTPToken(
            const std::string &label,
            const std::string &secret,
            Type type = TOTP,
            Algorithm algorithm = SHA1);

    /**
     * Constructs a new OTPToken with a custom digit length and period.
     *
     * By default a TOTP token with SHA-1 is created.
     */
    OTPToken(
            const std::string &label,
            const std::string &secret,
            std::uint8_t &digits,
            std::uint32_t &period,
            Type type = TOTP,
            Algorithm algorithm = SHA1);

    /**
     * Destructor
     */
    ~OTPToken();

    /**
     * Returns the type name of the token as string for display.
     * (TOTP, HOTP, Steam)
     *
     * This function can be used for interfaces to easily print out
     * the type without additional code bloat.
     */
    const std::string &typeName() const;

private:
    std::string _label;          // label
    std::string _secret;         // token secret
    std::uint8_t _digits = 6;    // number of digits
    std::uint32_t _period = 30;  // validity of token
    std::uint32_t _counter = 0;  // HOTP token counter
    Type _type = TOTP;           // token type
    Algorithm _algorithm = SHA1; // token algorithm
    Data icon;                   // raw icon data
};

#endif // OTPTOKEN_HPP
