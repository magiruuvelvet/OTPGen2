#include <bandit/bandit.h>
#include <benchmark.hpp>

#include <qr/decoder.hpp>
#include <qr/encoder.hpp>

using namespace snowhouse;
using namespace bandit;

go_bandit([]{
    describe("qr", []{
        const bool decoding_support = QRCode::supportsDecoding();
        const std::string test_assets_dir = TEST_ASSETS_DIR;

        benchmark_it("[decode no such file]", [&]{
            QRCode::Error error;
            const auto res = QRCode::decode(test_assets_dir + "/nosuchfile.png", &error);

            if (decoding_support)
            {
                AssertThat(error, Equals(QRCode::FileNotReadable));
                AssertThat(res, Equals(std::string()));
            }
            else
            {
                AssertThat(error, Equals(QRCode::DecodingNotSupported));
                AssertThat(res, Equals(std::string()));
            }
        });

        benchmark_it("[decode invalid file]", [&]{
            QRCode::Error error;
            const auto res = QRCode::decode(test_assets_dir + "/invalid.png", &error);

            if (decoding_support)
            {
                AssertThat(error, Equals(QRCode::DecodingError));
                AssertThat(res, Equals(std::string()));
            }
            else
            {
                AssertThat(error, Equals(QRCode::DecodingNotSupported));
                AssertThat(res, Equals(std::string()));
            }
        });

        benchmark_it("[decode]", [&]{
            QRCode::Error error;
            const auto res = QRCode::decode(test_assets_dir + "/qrcode.png", &error);

            if (decoding_support)
            {
                AssertThat(error, Equals(QRCode::NoError));
                AssertThat(res, Equals("otpauth://totp/Example:alice@google.com?secret=JBSWY3DPEHPK3PXP&issuer=Example"));
            }
            else
            {
                AssertThat(error, Equals(QRCode::DecodingNotSupported));
                AssertThat(res, Equals(std::string()));
            }
        });

        benchmark_it("[encode]", [&]{
            const auto res = QRCode::encode("hello world");
            AssertThat(res.size(), IsGreaterThanOrEqualTo(3000));
        });
    });
});
