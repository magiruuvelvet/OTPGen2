#include <bandit/bandit.h>
#include <benchmark.hpp>

#include <tokenstore.hpp>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("tokenstore", []{
        const std::string test_assets_dir = TEST_ASSETS_DIR;
        const std::string test_output_dir = TEST_OUTPUT_DIR;

        // create in-memory token store
        TokenStore tokenStore;

        benchmark_it("[delete password]", [&]{
            std::string password("some sensitive value");
            TokenStore::deletePassword(&password);
            AssertThat(password, Equals(std::string("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20)));
        });

        benchmark_it("[add]", [&]{
            bool res;

            res = tokenStore.addToken(OTPToken("test", "secret"));
            AssertThat(tokenStore.size(), Equals(1));
            AssertThat(res, Equals(true));

            res = tokenStore.addToken(OTPToken("test", "secret"));
            AssertThat(tokenStore.size(), Equals(1));
            AssertThat(res, Equals(true));

            res = tokenStore.addToken(OTPToken());
            AssertThat(tokenStore.size(), Equals(1));
            AssertThat(res, Equals(false));
        });

        benchmark_it("[remove]", [&]{
            tokenStore.removeToken(OTPToken());
            AssertThat(tokenStore.size(), Equals(1));

            tokenStore.removeToken(tokenStore.tokens()->at(0));
            AssertThat(tokenStore.size(), Equals(0));
        });

        benchmark_it("[decrypt]", [&]{
            TokenStore tks(test_assets_dir + "/test.tks", "password");
            AssertThat(tks.isValid(), Equals(true));
            AssertThat(tks.size(), Equals(2));
            AssertThat(tks[0].label(), Equals("test1"));
            AssertThat(tks[0].secret(), Equals("secret"));
            AssertThat(tks[1].label(), Equals("test2"));
            AssertThat(tks[1].secret(), Equals("secret"));
        });

        benchmark_it("[commit]", [&]{
            TokenStore tks(test_output_dir + "/commit_test.tks", "password");
            // note that duplicates aren't added so this test should succeed on repeated executions
            tks.addToken(OTPToken("test3", "secret"));
            const auto res = tks.commit();
            AssertThat(res, Equals(TokenStore::NoError));

            TokenStore tks2(test_output_dir + "/commit_test.tks", "password");
            AssertThat(tks2.isValid(), Equals(true));
            AssertThat(tks2.size(), Equals(1));
            AssertThat(tks2[0].label(), Equals("test3"));
            AssertThat(tks2[0].secret(), Equals("secret"));
        });
    });
});
