#include <bandit/bandit.h>

#include <otptoken.hpp>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("otptoken", []{
        OTPToken token("label", "secret", 7, 50, OTPToken::TOTP, OTPToken::SHA1);
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
    });
});
