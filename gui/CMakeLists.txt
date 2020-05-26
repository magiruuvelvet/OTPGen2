set(CURRENT_TARGET "gui")
set(CURRENT_TARGET_NAME "otpgen")
set(CURRENT_TARGET_INTERFACE "${CURRENT_TARGET}_interface")

message(STATUS "Configuring ${CURRENT_TARGET}...")

# Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools)
message(STATUS "Qt version: ${Qt5Core_VERSION}")
message(STATUS "Linguist tools found: ${Qt5LinguistTools_FOUND}")

set(CONFIG_STATUS_QT "${Qt5Core_VERSION} (system)" CACHE INTERNAL "")

# Qt automoc
message(STATUS "Enabling Qt automoc...")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Qt autorcc
message(STATUS "Enabling Qt autorcc...")
set(CMAKE_AUTORCC ON)

# embedded assets
qt5_add_resources(RCC_SOURCES "${PROJECT_SOURCE_DIR}/gui/assets/EmbeddedAssets.qrc")
add_custom_target(GenerateEmbeddedAssets DEPENDS ${RCC_SOURCES})

# ui translations
set(BUILD_TRANSLATIONS OFF CACHE BOOL "Build the UI translations.")
if (BUILD_TRANSLATIONS AND Qt5LinguistTools_FOUND)
    message(STATUS "Building of translations activated.")

    # qt lrelease binary
    get_target_property(qt5_lrelease_bin Qt5::lrelease IMPORTED_LOCATION)

    # scan available translation
    file(GLOB_RECURSE UI_TRANSLATION_FILES
        "i18n/*.ts"
    )

    # print some interesting variables
    message(STATUS "Compiled translations will be placed in: ${CMAKE_CURRENT_BINARY_DIR}/i18n")
    message(STATUS "lrelease location: ${qt5_lrelease_bin}")

    # copy qrc file
    configure_file("i18n/translations.qrc" "${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc" COPYONLY)

    # only compile ts files without updating them
    # translation files are updated manually outside of CMake as required
    # also there is a bug with Qt's translation macro which deletes all ts files on clean
    foreach (TS_FILE ${UI_TRANSLATION_FILES})
        get_filename_component(QM_OUTPUT_FILE "${TS_FILE}" NAME_WE)
        message(STATUS "Compiling translation ${QM_OUTPUT_FILE}...")
        execute_process(
            COMMAND "${qt5_lrelease_bin}" "${TS_FILE}" -silent -qm "${CMAKE_CURRENT_BINARY_DIR}/i18n/${QM_OUTPUT_FILE}.qm"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/i18n"
        )
    endforeach()

    qt5_add_resources(RCC_TRANSLATION_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc")
    add_custom_target(GenerateEmbeddedTranslations DEPENDS ${RCC_TRANSLATION_SOURCES})

    set(CONFIG_STATUS_TRANSLATIONS "enabled" CACHE INTERNAL "")

elseif (BUILD_TRANSLATIONS)
    message(WARNING "Building of translations requested but no Qt linguist tools found.")
    message(WARNING "Translations will not be built.")
    set(RCC_TRANSLATION_SOURCES "")
    add_custom_target(GenerateEmbeddedTranslations)
    set(CONFIG_STATUS_TRANSLATIONS "disabled automatically (missing tools)" CACHE INTERNAL "")
else()
    set(RCC_TRANSLATION_SOURCES "")
    add_custom_target(GenerateEmbeddedTranslations)
    set(CONFIG_STATUS_TRANSLATIONS "disabled" CACHE INTERNAL "")
endif()

# target must be created AFTER Qt cmake was run and automoc enabled
CreateTarget(${CURRENT_TARGET} EXECUTABLE ${CURRENT_TARGET_NAME} 20)

# append resource files
add_dependencies(${CURRENT_TARGET} GenerateEmbeddedTranslations GenerateEmbeddedAssets)
set_property(TARGET ${CURRENT_TARGET} APPEND PROPERTY SOURCES ${RCC_SOURCES} ${RCC_TRANSLATION_SOURCES})

# Qt deprecated warnings
target_compile_definitions(${CURRENT_TARGET} PRIVATE -DQT_DEPRECATED_WARNINGS)
target_compile_definitions(${CURRENT_TARGET} PRIVATE -DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

# disable Qt foreach macro
target_compile_definitions(${CURRENT_TARGET} PRIVATE -DQT_NO_FOREACH)

target_link_libraries(${CURRENT_TARGET}
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
    PRIVATE
        core_interface
        fmt
)

message(STATUS "Configured ${CURRENT_TARGET}.")
