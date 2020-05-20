#ifndef OTPTOKEN_HPP
#define OTPTOKEN_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

class OTPToken
{
public:

    // raw binary data container
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
        TOTP    = 1,
        HOTP    = 2,
        Steam   = 3,
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
            const std::uint8_t &digits,
            const std::uint32_t &period,
            const std::uint32_t &counter = 0,
            Type type = TOTP,
            Algorithm algorithm = SHA1);

    /**
     * Constructs a new OTPToken from serialized data.
     *
     * If deserialization fails an invalid object in constructed.
     *
     * @see serialize
     */
    OTPToken(const Data &data);

    /**
     * Destructor
     */
    ~OTPToken();

    /**
     * Checks if the OTPToken instance is valid.
     */
    inline bool isValid() const
    {
        return !this->_secret.empty();
    }

    inline const auto &label() const
    { return this->_label; }

    inline const auto &secret() const
    { return this->_secret; }

    inline const auto &digits() const
    { return this->_digits; }

    inline const auto &period() const
    { return this->_period; }

    inline const auto &counter() const
    { return this->_counter; }

    inline const auto &type() const
    { return this->_type; }

    inline const auto &algorithm() const
    { return this->_algorithm; }

    inline const auto &icon() const
    { return this->_icon; }

    /**
     * Returns the type name of the token as string for display.
     * (TOTP, HOTP, Steam)
     *
     * This function can be used for interfaces to easily print out
     * the type without additional code bloat.
     */
    const std::string &typeName() const;

    /**
     * Serializes the object into a portable binary sequence.
     *
     * To construct a OTPToken object again use the data constructor.
     */
    const Data serialize() const;

    /**
     * Generate token from current time.
     */
    const std::string generate(Error *error = nullptr) const;

    /**
     * Generate token from the given time.
     */
    const std::string generate(const std::time_t &time, Error *error = nullptr) const;

private:
    OTPToken() {}

    // Token Property Version (increment if changing properties)
    static const std::uint32_t VERSION;

    // Serialization Support
    template<class Archive>
    friend void save(Archive &archive, const OTPToken &token);
    template<class Archive>
    friend void load(Archive &archive, OTPToken &token);

    // Token Properties
    std::string _label;          // label
    std::string _secret;         // token secret
    std::uint8_t _digits = 6;    // number of digits
    std::uint32_t _period = 30;  // validity of token
    std::uint32_t _counter = 0;  // HOTP token counter
    Type _type = TOTP;           // token type
    Algorithm _algorithm = SHA1; // token algorithm
    Data _icon;                  // raw icon data
};

#endif // OTPTOKEN_HPP
