#include "encoder.hpp"

#include <QrCode.hpp>

using qrcodegen::QrCode;

const std::string QRCode::encode(const std::string &data)
{
    const QrCode qr = QrCode::encodeText(data.c_str(), QrCode::Ecc::MEDIUM);
    return qr.toSvgString(4);
}
