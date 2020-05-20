#ifndef CORE_PRIVATE_SERIALIZE_HPP
#define CORE_PRIVATE_SERIALIZE_HPP

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <otptoken.hpp>

template<class Archive>
void save(Archive &archive, const OTPToken &token)
{
    archive(
        OTPToken::VERSION,
        token._label,
        token._secret,
        token._digits,
        token._period,
        token._counter,
        token._type,
        token._algorithm,
        token._icon
    );
}

template<class Archive>
void load(Archive &archive, OTPToken &token)
{
    // TODO: version check

    std::uint32_t version;
    archive(
        version,
        token._label,
        token._secret,
        token._digits,
        token._period,
        token._counter,
        token._type,
        token._algorithm,
        token._icon
    );
}

#endif // CORE_PRIVATE_SERIALIZE_HPP
