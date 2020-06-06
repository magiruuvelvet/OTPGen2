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

#include <magic_enum.hpp>

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
        file.open(filePath, base_flags | std::ios_base::out | std::ios_base::trunc);
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
        file.close();

        // convert to std::string
        contents = std::string(buffer.data(), buffer.size());
        return true;
    }

    return false;
}

static bool writeFile(const std::string &filePath, const std::string &contents)
{
    std::fstream file;
    file.open(filePath, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
    if (file.is_open())
    {
        file.write(contents.data(), contents.size());
        file.flush();
        file.close();
        return true;
    }

    return false;
}

static CryptoPP::SecByteBlock makeKey(const std::string &password)
{
    const unsigned int aes_max_keylength = CryptoPP::AES::MAX_KEYLENGTH;
    const unsigned int aes_blocksize = CryptoPP::AES::BLOCKSIZE;

    CryptoPP::SecByteBlock key(aes_max_keylength + aes_blocksize);
    CryptoPP::HKDF<CryptoPP::SHA256> hkdf;
    hkdf.DeriveKey(key, key.size(),
                   reinterpret_cast<const unsigned char*>(password.data()), password.size(),
                   reinterpret_cast<const unsigned char*>(password.data()), password.size(), nullptr, 0);

    return key;
}

static bool encryptData(const std::ostringstream &input, const std::string &password, std::string &output)
{
    output.clear();

    try {

        const std::string inputData = input.str();
        const auto key = makeKey(password);

        CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(output));
        stfEncryptor.Put(reinterpret_cast<const unsigned char*>(inputData.data()), inputData.size());
        stfEncryptor.MessageEnd();

        return true;

    } catch (...) {
        output.clear();
    }

    return false;
}

static bool decryptData(const std::string &encrypted, const std::string &password, std::string &decrypted)
{
    decrypted.clear();

    try {

        const auto key = makeKey(password);

        CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
        CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, reinterpret_cast<const unsigned char*>(password.data()));

        CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decrypted));
        stfDecryptor.Put(reinterpret_cast<const unsigned char*>(encrypted.data()), encrypted.size());
        stfDecryptor.MessageEnd();

        return true;

    } catch (...) {
        decrypted.clear();
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

    std::string fileContents;

    if (exists && is_reg)
    {
        // attempt to use existing file
        if (loadFileContents(this->_filePath, fileContents))
        {
            if (!fileContents.empty())
            {
                std::string decrypted;
                if (decryptData(fileContents, this->_password, decrypted))
                {
                    this->deserializeData(decrypted);
                    return;
                }
                else
                {
                    this->_state = DecryptionError;
                    return;
                }
            }
        }
        else
        {
            this->_state = PermissionDenied;
            return;
        }
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
        if (!loadFileContents(this->_filePath, fileContents, true))
        {
            this->_state = PermissionDenied;
            return;
        }
    }

    this->_state = NoError;
}

TokenStore::~TokenStore()
{
    // zero fill hashed password on destruction
    TokenStore::deletePassword(&this->_password);
}

bool TokenStore::addToken(const OTPToken &newToken)
{
    if (!newToken.canGenerateTokens())
    {
        return false;
    }

    // check if token already exists in store
    for (auto&& token : this->_tokens)
    {
        if (token == newToken)
        {
            return true;
        }
    }

    this->_tokens.emplace_back(newToken);
    return true;
}

void TokenStore::removeToken(const OTPToken &token)
{
    for (auto i = 0; i < this->_tokens.size(); ++i)
    {
        if (this->_tokens.at(i) == token)
        {
            this->_tokens.erase(this->_tokens.begin() + i);
            break;
        }
    }
}

const std::string_view TokenStore::error_code() const
{
    return magic_enum::enum_name(this->_state);
}

TokenStore::ErrorCode TokenStore::commit()
{
    // refuse to commit a invalid state
    if (!this->isValid())
    {
        return this->_state;
    }

    // serialize the entire thing
    std::ostringstream serializedTokensBuf;
    cereal::PortableBinaryOutputArchive archive(serializedTokensBuf);
    archive(this->_tokens);

    // encrypt the serialized data
    std::string encryptedData;
    if (!encryptData(serializedTokensBuf, this->_password, encryptedData))
    {
        return EncryptionError;
    }

    // write file to disk
    if (!writeFile(this->_filePath, encryptedData))
    {
        return PermissionDenied;
    }

    return NoError;
}

void TokenStore::deserializeData(const std::string &fileContents)
{
    std::istringstream buffer(fileContents);

    cereal::PortableBinaryInputArchive archive(buffer);
    try {
        archive(this->_tokens);
        for (auto&& token : this->_tokens)
        {
            if (!token._secret.empty())
            {
                token.valid = true;
            }
        }
    } catch (cereal::Exception &e) {
        this->_tokens.clear();
        this->_state = DeserializationError;
    }
}
