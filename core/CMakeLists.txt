set(CURRENT_TARGET "core")
set(CURRENT_TARGET_NAME "libotpgen")
set(CURRENT_TARGET_INTERFACE "${CURRENT_TARGET}_interface")

message(STATUS "Configuring ${CURRENT_TARGET}...")

CreateTarget(${CURRENT_TARGET} SHARED ${CURRENT_TARGET_NAME} 20)

# cereal
target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/libs/cereal/include")

# crypto++
set(BUNDLED_CRYPTOPP OFF CACHE BOOL "Use the bundled crypto++ library.")
if (BUNDLED_CRYPTOPP OR NOT PKG_CONFIG_FOUND)
    message(STATUS "Building with bundled crypto++")
    message(STATUS "Configuring bundled crypto++ library...")
    add_subdirectory("${PROJECT_SOURCE_DIR}/libs/cryptopp" "${CMAKE_CURRENT_BINARY_DIR}/cryptopp" EXCLUDE_FROM_ALL)
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/libs/cryptopp")
    target_link_libraries(${CURRENT_TARGET} cryptopp-static)
    message(STATUS "Configured bundled crypto++ library.")
else()
    pkg_check_modules(CRYPTOPP REQUIRED libcryptopp)
    target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${CRYPTOPP_INCLUDEDIR}")
    target_link_libraries(${CURRENT_TARGET} ${CRYPTOPP_LDFLAGS})
endif()

add_library(${CURRENT_TARGET_INTERFACE} INTERFACE)
target_include_directories(${CURRENT_TARGET_INTERFACE} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${CURRENT_TARGET_INTERFACE} INTERFACE ${CURRENT_TARGET})

message(STATUS "Configured ${CURRENT_TARGET}.")
