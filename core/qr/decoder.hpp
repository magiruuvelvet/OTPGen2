#ifndef QRCODE_DECODER
#define QRCODE_DECODER

#include <string>

namespace QRCode
{
    enum Error
    {
        NoError = 0,            // success, no errors reported
        FileNotReadable,        // file not found or not readable
        DecodingNotSupported,   // compiled without decoding support
        DecodingError,          // no valid barcode found in image
        NoBarcodesFound,        // no barcode found at all in image (blank image)
    };

    /**
     * Checks if the library was built with QR code decoding support.
     * If this function returns false, all other decoding functions
     * return empty results and set the optimal error code to DecodingNotSupported.
     */
    bool supportsDecoding();

    /**
     * Decodes the given QR code and returns its plain text contents as string.
     * This function may return binary data wrapped into a string.
     */
    const std::string decode(const std::string &filePath, Error *error = nullptr);
}

#endif // QRCODE_DECODER
