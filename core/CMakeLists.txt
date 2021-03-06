set(CURRENT_TARGET "core")
set(CURRENT_TARGET_NAME "libotpgen")
set(CURRENT_TARGET_INTERFACE "${CURRENT_TARGET}_interface")

message(STATUS "Configuring ${CURRENT_TARGET}...")

CreateTarget(${CURRENT_TARGET} SHARED ${CURRENT_TARGET_NAME} C++ 20)

# cereal
set(BUNDLED_CEREAL ON CACHE BOOL "Use the bundled cereal header-only library.")
if (BUNDLED_CEREAL)
    message(STATUS "Using bundled cereal")
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/libs/cereal/include")
    set(CONFIG_STATUS_CEREAL "bundled" CACHE INTERNAL "")
else()
    message(STATUS "Using shared cereal")
    find_package(cereal REQUIRED)
    get_target_property(CEREAL_INCLUDE_DIR cereal INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${CEREAL_INCLUDE_DIR}")
    set(CONFIG_STATUS_CEREAL "system" CACHE INTERNAL "")
endif()

# crypto++
set(BUNDLED_CRYPTOPP OFF CACHE BOOL "Use the bundled crypto++ library.")
if (BUNDLED_CRYPTOPP OR NOT PKG_CONFIG_FOUND)
    message(STATUS "Building with bundled crypto++")
    message(STATUS "Configuring bundled crypto++ library...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/libs/cryptopp" "${CMAKE_CURRENT_BINARY_DIR}/cryptopp" EXCLUDE_FROM_ALL)
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/libs/cryptopp")
    target_link_libraries(${CURRENT_TARGET} PRIVATE cryptopp-static)
    message(STATUS "Configured bundled crypto++ library.")
    set(CONFIG_STATUS_CRYPTOPP "8.2 (bundled)" CACHE INTERNAL "")
else()
    message(STATUS "Building with shared crypto++")
    pkg_check_modules(CRYPTOPP REQUIRED "libcryptopp>=8.2")
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries(${CURRENT_TARGET} PRIVATE ${CRYPTOPP_LDFLAGS})
    set(CONFIG_STATUS_CRYPTOPP "${CRYPTOPP_VERSION} (system)" CACHE INTERNAL "")
endif()

# QR code decoding support (requires zbar and ImageMagick C++)
set(QRCODE_DECODING_SUPPORT ON CACHE BOOL "Add support for decoding QR codes.")
if (QRCODE_DECODING_SUPPORT AND PKG_CONFIG_FOUND)
    pkg_check_modules(ZBAR REQUIRED "zbar>=0.23")
    pkg_check_modules(MAGICKPP REQUIRED "Magick++")
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${ZBAR_INCLUDEDIR}")
    target_link_libraries(${CURRENT_TARGET} PRIVATE ${ZBAR_LDFLAGS})
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${MAGICKPP_INCLUDEDIR}")
    target_link_libraries(${CURRENT_TARGET} PRIVATE ${MAGICKPP_LDFLAGS})
    target_compile_definitions(${CURRENT_TARGET} PRIVATE -DQRCODE_DECODING_SUPPORT=1)
    message(STATUS "QR code decoding support enabled.")
    set(CONFIG_STATUS_QRCODEDECODING "yes" CACHE INTERNAL "")
    set(CONFIG_STATUS_ZBAR "${ZBAR_VERSION} (system)" CACHE INTERNAL "")
    set(CONFIG_STATUS_MAGICKPP "${MAGICKPP_VERSION} (system)" CACHE INTERNAL "")
elseif (QRCODE_DECODING_SUPPORT)
    message(FATAL_ERROR "QR Code decoding support requested but pkg-config not present on system.")
else()
    # disable QR code decoding support
    message(STATUS "QR code decoding support disabled.")
    target_compile_definitions(${CURRENT_TARGET} PRIVATE -DQRCODE_DECODING_SUPPORT=0)
    set(CONFIG_STATUS_QRCODEDECODING "no" CACHE INTERNAL "")
    set(CONFIG_STATUS_ZBAR "(not needed)" CACHE INTERNAL "")
    set(CONFIG_STATUS_MAGICKPP "(not needed)" CACHE INTERNAL "")
endif()

target_link_libraries(${CURRENT_TARGET}
    PRIVATE
        qr-code-generator
        magic_enum
        fmt
)

add_library(${CURRENT_TARGET_INTERFACE} INTERFACE)
target_include_directories(${CURRENT_TARGET_INTERFACE} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${CURRENT_TARGET_INTERFACE} INTERFACE ${CURRENT_TARGET})

message(STATUS "Configured ${CURRENT_TARGET}.")
