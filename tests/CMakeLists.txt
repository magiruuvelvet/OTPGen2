set(CURRENT_TARGET "tests")
set(CURRENT_TARGET_NAME "otpgen-tests")

message(STATUS "Configuring ${CURRENT_TARGET}...")

CreateTarget(${CURRENT_TARGET} EXECUTABLE ${CURRENT_TARGET_NAME} C++ 20)

target_link_libraries(${CURRENT_TARGET} PRIVATE libs::core fmt)

target_include_directories(${CURRENT_TARGET} SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/libs/bandit")

target_compile_definitions(${CURRENT_TARGET} PRIVATE "-DTEST_ASSETS_DIR=\"${PROJECT_SOURCE_DIR}/tests/test_assets\"")
target_compile_definitions(${CURRENT_TARGET} PRIVATE "-DTEST_OUTPUT_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"")
