#include <bandit/bandit.h>

#include <otptoken.hpp>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("otptoken", []{
        OTPToken token("label", "secret", 7, 50, 0, OTPToken::TOTP, OTPToken::SHA1);
        OTPToken::Data serialized;

        it("[serialize]", [&]{
            serialized = token.serialize();

            // portable binary test
            const OTPToken::Data results = {
                0x01, 0x01, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x6C, 0x61, 0x62, 0x65, 0x6C, 0x06, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x65, 0x63, 0x72,
                0x65, 0x74, 0x07, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            };

            AssertThat(serialized, Equals(results));
        });

        it("[deserialize]", [&]{
            OTPToken deserialized(serialized);

            AssertThat(deserialized.isValid(), Equals(true));

            AssertThat(deserialized.label(), Equals("label"));
            AssertThat(deserialized.secret(), Equals("secret"));
            AssertThat(deserialized.digits(), Equals(7));
            AssertThat(deserialized.period(), Equals(50));
            AssertThat(deserialized.counter(), Equals(0));
            AssertThat(deserialized.type(), Equals(OTPToken::TOTP));
            AssertThat(deserialized.algorithm(), Equals(OTPToken::SHA1));
            AssertThat(deserialized.icon(), Equals(OTPToken::Data()));
            AssertThat(deserialized.icon().size(), Equals(0));
        });

        it("[deserialize invalid]", [&]{
            OTPToken deserialized(OTPToken::Data{0x01, 0x30, 0x30});
            AssertThat(deserialized.isValid(), Equals(false));
        });

        // test totp token at a fixed time, result must be always the same
        it("[compute TOTP 1]", [&]{
            OTPToken tkn("", "XYZA123456KDDK83D", OTPToken::TOTP, OTPToken::SHA1);
            AssertThat(tkn.generate(1536573862), Equals(std::string("122810")));
        });

        // test totp token at a fixed time, result must be always the same
        it("[compute TOTP 2]", [&]{
            OTPToken tkn("", "XYZA123456KDDK83D28273", 7, 10, 0, OTPToken::TOTP, OTPToken::SHA1); // Authy uses those values
            AssertThat(tkn.generate(1536573862), Equals(std::string("8578249")));
        });

        // test hotp token with a fixed counter at 12
        it("[compute HOTP]", [&]{
            OTPToken tkn("", "XYZA123456KDDK83D", 6, 0, 12, OTPToken::HOTP, OTPToken::SHA1);
            AssertThat(tkn.generate(), Equals(std::string("534003")));
        });

        // test steam token at a fixed time, result must be always the same
        it("[compute Steam]", [&]{
            OTPToken tkn("", "ABC30WAY33X57CCBU3EAXGDDMX35S39M", OTPToken::Steam);
            AssertThat(tkn.generate(1536573862), Equals(std::string("GQTTM")));
        });
    });
});
