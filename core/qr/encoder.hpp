#ifndef QRCODE_ENCODER
#define QRCODE_ENCODER

#include <string>

namespace QRCode
{
    /**
     * Encodes the given data into a QR code and returns
     * it as SVG with medium error correction.
     */
    const std::string encode(const std::string &data);
}

#endif // QRCODE_ENCODER
