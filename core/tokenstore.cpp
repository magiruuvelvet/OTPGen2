#include "tokenstore.hpp"
#include "private/serialize.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

#include <cryptopp/cryptlib.h>
#include <cryptopp/algparam.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

TokenStore::TokenStore(const std::string &filePath, const std::string &password)
    : _filePath(filePath)
{
    // TODO: file handling

    // store password in hashed form and use that for the actual token store password
    if (!password.empty())
    {
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource src(password, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::Base64Encoder(
                    new CryptoPP::StringSink(this->_password))));
    }
}

TokenStore::~TokenStore()
{
    // zero fill hashed password on destruction
    std::fill(this->_password.begin(), this->_password.end(), 0);
}

bool TokenStore::isValid() const
{
    // TODO
    return false;
}

bool TokenStore::commit()
{
    // refuse to commit a invalid state
    if (!this->isValid())
    {
        return false;
    }

    // serialize the entire thing
    std::ostringstream buffer;
    cereal::PortableBinaryOutputArchive archive(buffer);
    archive(this->_tokens);

    // TODO: now encrypt it

    return false;
}
