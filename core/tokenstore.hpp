#ifndef TOKENSTORE_HPP
#define TOKENSTORE_HPP

#include <string>
#include <vector>

#include "otptoken.hpp"

class TokenStore
{
public:

    /**
     * Constructs a new token store using the given file path.
     * If the file doesn't exist yet, a new store will be initialized.
     * On existing files the tokens will be loaded from that file.
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
    inline const std::vector<OTPToken> *tokens() const
    {
        return &this->_tokens;
    }

    /**
     * Adds a new token to the token store.
     * The token object is copy constructed.
     */
    inline void addToken(const OTPToken &token)
    {
        this->_tokens.emplace_back(token);
    }

    /**
     * Checks if this token store is properly initialized.
     */
    bool isValid() const;

    /**
     * Commit changes to the filesystem.
     * This method must be explicitly called or all unsaved changes are lost.
     */
    bool commit();

private:
    std::string _filePath;
    std::string _password; // TODO: check security (memory scanning, coredumps, etc.)
    std::vector<OTPToken> _tokens;
};

#endif // TOKENSTORE_HPP
