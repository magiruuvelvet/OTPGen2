#include "tokenstore.hpp"
#include "private/serialize.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>

#include <cryptopp/cryptlib.h>
#include <cryptopp/algparam.h>
#include <cryptopp/aes.h>
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

namespace
{

static bool loadFileContents(const std::string filePath, std::string &contents, bool createMode = false)
{
    // ensure the given string is empty
    contents.clear();

    const auto base_flags = std::ios_base::binary;

    if (createMode)
    {
        std::fstream file;
        file.open(filePath, base_flags | std::ios_base::out);
        if (file.is_open())
        {
            file.close();
            return true;
        }
        else
        {
            return false;
        }
    }

    std::fstream file;
    file.open(filePath, base_flags | std::ios_base::in | std::ios_base::ate);
    if (file.is_open())
    {
        // get file size
        const auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        // read entire file
        std::vector<char> buffer(size);
        file.read(&buffer[0], size);

        // convert to std::string
        contents = std::string(buffer.data(), buffer.size());
        return true;
    }

    return false;
}

} // anonymous namespace

void TokenStore::deletePassword(std::string *password)
{
    std::fill(password->begin(), password->end(), 0);
}

TokenStore::TokenStore(const std::string &filePath, const std::string &password)
    : _filePath(filePath)
{
    // store password in hashed form and use that for the actual token store password
    if (!password.empty())
    {
        CryptoPP::SHA256 hash;
        CryptoPP::StringSource src(password, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::Base64Encoder(
                    new CryptoPP::StringSink(this->_password))));
        this->_state = NoError;
    }
    else
    {
        this->_state = EmptyPassword;
        return;
    }

    const bool exists = std::filesystem::exists(filePath);
    const bool is_reg = std::filesystem::is_regular_file(filePath);

    if (exists && is_reg)
    {
        // attempt to use existing file
        return;
    }
    else if (exists)
    {
        // file exists but not a regular file
        this->_state = NotARegularFile;
        return;
    }
    else
    {
        // attempt to create a new file
        return;
    }
}

TokenStore::~TokenStore()
{
    // zero fill hashed password on destruction
    TokenStore::deletePassword(&this->_password);
}

TokenStore::ErrorCode TokenStore::commit()
{
    // refuse to commit a invalid state
    if (!this->isValid())
    {
        return this->_state;
    }

    // serialize the entire thing
    std::ostringstream buffer;
    cereal::PortableBinaryOutputArchive archive(buffer);
    archive(this->_tokens);

    // TODO: now encrypt it

    return EncryptionError;
}
