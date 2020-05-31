#ifndef TOKENSTORE_HPP
#define TOKENSTORE_HPP

#include <string>
#include <vector>

#include "otptoken.hpp"

class TokenStore
{
public:

    enum ErrorCode
    {
        NoError,                // no errors
        MemoryOnly,             // token store is memory-only and has no file linked to it
        PermissionDenied,       // file isn't read or writable
        NotARegularFile,        // file is a directory or something else
        EncryptionError,        // error during encryption occurred
        DecryptionError,        // error during decryption occurred (possible reasons are an incorrect password or an invalid ciphertext)
        EmptyPassword,          // no password for enc/dec supplied
        DeserializationError,   // error occurred during deserialization of the decrypted data
    };

    /**
     * Zero fills the given string.
     */
    static void deletePassword(std::string *password);

    /**
     * Constructs a new token store in memory without file operations.
     * Calling @see commit does nothing when using this mode.
     * Calling @see isValid always returns false. Explicitly check for
     * the `MemoryOnly` state instead when using this mode.
     */
    TokenStore()
    {
    }

    /**
     * Constructs a new token store using the given file path.
     * If the file doesn't exist yet, a new store will be initialized.
     * On existing files the tokens will be loaded from that file.
     *
     * Note: missing parent directories are not being created
     *
     * Use @see isValid to check if initialization worked.
     */
    TokenStore(const std::string &filePath, const std::string &password);

    /**
     * Destroys the token store.
     */
    ~TokenStore();

    /**
     * Returns a const pointer to all stored tokens.
     */
    constexpr inline const std::vector<OTPToken> *tokens() const
    {
        return &this->_tokens;
    }

    /**
     * Adds a new token to the token store.
     * The token object is copy constructed.
     */
    constexpr inline void addToken(const OTPToken &token)
    {
        this->_tokens.emplace_back(token);
    }

    /**
     * Checks if this token store is properly initialized.
     */
    constexpr inline bool isValid() const
    {
        return this->_state == NoError;
    }

    /**
     * Receive the last error state.
     *
     * Use this if @see isValid returns false for error handling.
     */
    constexpr inline ErrorCode state() const
    {
        return this->_state;
    }

    /**
     * Commit changes to the filesystem.
     * This method must be explicitly called or all unsaved changes are lost.
     */
    ErrorCode commit();

private:
    std::string _filePath;
    std::string _password;
    std::vector<OTPToken> _tokens;

    ErrorCode _state = MemoryOnly;
};

#endif // TOKENSTORE_HPP
