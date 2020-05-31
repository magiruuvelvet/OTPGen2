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
        InvalidBase64Input, // for Steam tokens
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
     * Constructs an invalid instance.
     */
    OTPToken()
        : valid(false)
    {
    }

    /**
     * Destructor
     */
    ~OTPToken();

    /**
     * Checks if the OTPToken instance is considered valid
     * and has all data required to generate tokens.
     */
    constexpr inline bool isValid() const
    {
        return this->valid && !this->_secret.empty();
    }

    /**
     * Checks if the OTPToken instance can generate non-empty tokens.
     * This method also calls @see isValid internally.
     */
    inline bool canGenerateTokens() const
    {
        return !this->generate().empty();
    }

    constexpr inline void setLabel(const std::string &label)
    { this->_label = label; }
    constexpr inline const auto &label() const
    { return this->_label; }

    constexpr inline void setSecret(const std::string &secret)
    { this->_secret = secret; }
    constexpr inline const auto &secret() const
    { return this->_secret; }

    constexpr inline void setDigits(const std::uint8_t &digits)
    { this->_digits = digits; }
    constexpr inline const auto &digits() const
    { return this->_digits; }

    constexpr inline void setPeriod(const std::uint32_t &period)
    { this->_period = period; }
    constexpr inline const auto &period() const
    { return this->_period; }

    constexpr inline void setCounter(const std::uint32_t &counter)
    { this->_counter = counter; }
    constexpr inline const auto &counter() const
    { return this->_counter; }

    constexpr inline void setType(const Type &type)
    { this->_type = type; }
    constexpr inline const auto &type() const
    { return this->_type; }

    constexpr inline void setAlgorithm(const Algorithm &algorithm)
    { this->_algorithm = algorithm; }
    constexpr inline const auto &algorithm() const
    { return this->_algorithm; }

    constexpr inline void setIcon(const Data &icon)
    { this->_icon = icon; }
    constexpr inline const auto &icon() const
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

    /**
     * Calculates the remaining token validity from the current system time.
     * The returned time is in seconds.
     *
     * This function always returns 0 on HOTP tokens.
     */
    const std::uint64_t remainingTokenValidity() const;

    /**
     * equal operator
     */
    inline const bool operator== (const OTPToken &other) const
    {
        return
            this->_label == other._label &&
            this->_secret == other._secret &&
            this->_digits == other._digits &&
            this->_period == other._period &&
            this->_counter == other._counter &&
            this->_type == other._type &&
            this->_algorithm == other._algorithm &&
            this->_icon == other._icon;
    }

    /**
     * does not equal operator
     */
    constexpr inline const bool operator!= (const OTPToken &other) const
    {
        return !this->operator== (other);
    }

private:
    // Token Property Version (increment if changing properties)
    static const std::uint32_t VERSION;

    friend class TokenStore;

    // Serialization Support
    template<class Archive>
    friend void save(Archive &archive, const OTPToken &token);
    template<class Archive>
    friend void load(Archive &archive, OTPToken &token);

    // internal function to set token type defaults
    void set_defaults(const void *def);

    // Token Properties
    std::string _label;     // label
    std::string _secret;    // token secret
    std::uint8_t _digits;   // number of digits
    std::uint32_t _period;  // validity of token
    std::uint32_t _counter; // HOTP token counter
    Type _type;             // token type
    Algorithm _algorithm;   // token algorithm
    Data _icon;             // raw icon data

    // internal validity state for deserialized instances
    bool valid = true;
};

#endif // OTPTOKEN_HPP
