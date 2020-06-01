#include <bandit/bandit.h>

#include <tokenstore.hpp>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("tokenstore", []{

        // create in-memory token store
        TokenStore tokenStore;

        it("[delete password]", [&]{
            std::string password("some sensitive value");
            TokenStore::deletePassword(&password);
            AssertThat(password, Equals(std::string("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20)));
        });

        it("[add]", [&]{
            tokenStore.addToken(OTPToken("test", "secret"));
            AssertThat(tokenStore.size(), Equals(1));

            tokenStore.addToken(OTPToken("test", "secret"));
            AssertThat(tokenStore.size(), Equals(1));
        });

        it("[remove]", [&]{
            tokenStore.removeToken(OTPToken());
            AssertThat(tokenStore.size(), Equals(1));

            tokenStore.removeToken(tokenStore.tokens()->at(0));
            AssertThat(tokenStore.size(), Equals(0));
        });
    });
});
