cmake_minimum_required(VERSION 3.14)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

include(ProjectSetup)

project(OTPGen CXX)

DetectAdditionalDebugOptions()

# pkg-config (required to find shared dependencies)
find_package(PkgConfig)
if (NOT PKG_CONFIG_FOUND)
    message(WARNING "pkg-config executable not found on your system!")
    message(WARNING "pkg-config based dependencies will not be found and bundled ones are used as fallback.")
endif()

# include dependencies
add_subdirectory(libs)

# shared core library (libotpgen)
add_subdirectory(core)

# Qt frontend
add_subdirectory(gui)

# unit tests
set(ENABLE_TESTING OFF CACHE BOOL "Build the unit tests.")
if (ENABLE_TESTING)
    message(STATUS "Testing enabled.")
    add_subdirectory(tests)
    set(CONFIG_STATUS_TESTS "enabled" CACHE INTERNAL "")
else()
    set(CONFIG_STATUS_TESTS "disabled" CACHE INTERNAL "")
endif()



# print configuration summary
message(STATUS "")
message(STATUS "============ Configuration Summary ============")

message(STATUS "Build Type:                ${CMAKE_BUILD_TYPE}")
message(STATUS "OS:                        ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Compiler:                  ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "pkg-config available:      ${PKG_CONFIG_FOUND}")

message(STATUS "Unit Tests:                ${CONFIG_STATUS_TESTS}")
message(STATUS "QR Code decoding support:  ${CONFIG_STATUS_QRCODEDECODING}")
message(STATUS "crypto++:                  ${CONFIG_STATUS_CRYPTOPP}")
message(STATUS "cereal:                    ${CONFIG_STATUS_CEREAL}")
message(STATUS "libfmt:                    ${CONFIG_STATUS_LIBFMT}")
message(STATUS "zbar:                      ${CONFIG_STATUS_ZBAR}")
message(STATUS "Magick++:                  ${CONFIG_STATUS_MAGICKPP}")
message(STATUS "Qt:                        ${CONFIG_STATUS_QT}")
message(STATUS "Translations:              ${CONFIG_STATUS_TRANSLATIONS}")

message(STATUS "")
