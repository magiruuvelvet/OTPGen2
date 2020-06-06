# fmt
set(BUNDLED_LIBFMT ON CACHE BOOL "Use the bundled libfmt.")
if (BUNDLED_LIBFMT)
    message(STATUS "Building with bundled libfmt")
    message(STATUS "Configuring bundled libfmt...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/libs/fmt" "${PROJECT_BINARY_DIR}/libs/fmt" EXCLUDE_FROM_ALL)
    message(STATUS "Configured bundled libfmt.")
    set(CONFIG_STATUS_LIBFMT "6.2.1 (bundled)" CACHE INTERNAL "")
else()
    message(STATUS "Building with shared libfmt")
    find_package(fmt 6.2 REQUIRED)
    message(STATUS "Found libfmt ${fmt_VERSION}")
    set(CONFIG_STATUS_LIBFMT "${fmt_VERSION} (system)" CACHE INTERNAL "")
endif()

# qr-code-generator
add_library(qr-code-generator-lib STATIC
    "${PROJECT_SOURCE_DIR}/libs/qr-code-generator/cpp/QrCode.cpp"
    "${PROJECT_SOURCE_DIR}/libs/qr-code-generator/cpp/QrCode.hpp"
)
set_target_properties(qr-code-generator-lib PROPERTIES PREFIX "")
set_target_properties(qr-code-generator-lib PROPERTIES OUTPUT_NAME "qr-code-generator")
SetCppStandard(qr-code-generator-lib 20)
set_target_properties(qr-code-generator-lib PROPERTIES LINKER_LANGUAGE CXX)
add_library(qr-code-generator INTERFACE)
target_include_directories(qr-code-generator INTERFACE "${PROJECT_SOURCE_DIR}/libs/qr-code-generator/cpp")
target_link_libraries(qr-code-generator INTERFACE qr-code-generator-lib)

# magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE "${PROJECT_SOURCE_DIR}/libs/magic_enum/include")
