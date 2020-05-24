#include "decoder.hpp"

#if QRCODE_DECODING_SUPPORT
#define MAGICKCORE_QUANTUM_DEPTH 8
#define MAGICKCORE_HDRI_ENABLE 1
#include <Magick++.h>
#include <zbar.h>

#include <memory>
#endif

namespace
{

static inline void set_error(QRCode::Error *error, QRCode::Error value)
{
    if (error)
    {
        (*error) = value;
    }
}

} // anonymous namespace

bool QRCode::supportsDecoding()
{
#if QRCODE_DECODING_SUPPORT
    return true;
#else
    return false;
#endif
}

const std::string QRCode::decode(const std::string &filePath, Error *error)
{
    if (!supportsDecoding())
    {
        set_error(error, DecodingNotSupported);
        return {};
    }

#if QRCODE_DECODING_SUPPORT

    set_error(error, NoError);

    // read image using ImageMagick
    std::unique_ptr<Magick::Image> magick;
    try {
        magick = std::make_unique<Magick::Image>(filePath);
        if (!magick->isValid())
        {
            set_error(error, FileNotReadable);
            return {};
        }
    } catch (...) {
        set_error(error, FileNotReadable);
        return {};
    }

    // convert and extract raw 8-bit gray style pixels
    Magick::Blob blob;
    magick->modifyImage();
    magick->write(&blob, "GRAY", 8);
    const void *raw = blob.data();
    const auto width = magick->columns();
    const auto height = magick->rows();

    // create zbar image instance
    zbar::Image image(width, height, "Y800", raw, width * height);

    // scan the image for barcodes
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    auto res = scanner.scan(image);

    if (res != 1)
    {
        set_error(error, DecodingError);
        return {};
    }

    // extract results
    for (zbar::Image::SymbolIterator symbol = image.symbol_begin();
         symbol != image.symbol_end();
         ++symbol)
    {
        // symbol->get_type_name() : QR-Code (bar code type)
        return symbol->get_data(); // decoded plain text
    }

    set_error(error, NoBarcodesFound);
    return {};

#else
    set_error(error, DecodingNotSupported);
    return {};
#endif
}
