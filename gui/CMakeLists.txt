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

# Qt automoc
message(STATUS "Enabling Qt automoc...")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Qt autorcc
message(STATUS "Enabling Qt autorcc...")
set(CMAKE_AUTORCC ON)

# ui translations
set(BUILD_TRANSLATIONS OFF CACHE BOOL "Build the UI translations.")
if (BUILD_TRANSLATIONS AND Qt5LinguistTools_FOUND)
    message(STATUS "Building of translations activated.")

    # scan available translation
    file(GLOB_RECURSE UI_TRANSLATION_FILES
        "i18n/*.ts"
    )

    # lingust must know the source files, so scan them again
    file(GLOB_RECURSE UI_SOURCE_FILES
        "*.cpp"
        "*.hpp"
    )

    # set output location of built translation files
    set_source_files_properties(${UI_TRANSLATION_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/i18n")

    # copy qrc file
    configure_file("i18n/translations.qrc" "${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc" COPYONLY)

    qt5_create_translation(QM_FILES
        ${UI_SOURCE_FILES}            # source files; tr()
        ${UI_TRANSLATION_FILES}       # translation files (.ts)
    )

    qt5_add_resources(RCC_TRANSLATION_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/i18n/translations.qrc")
    add_custom_target(GenerateEmbeddedTranslations DEPENDS ${RCC_TRANSLATION_SOURCES})

elseif (BUILD_TRANSLATIONS)
    message(WARNING "Building of translations requested but no Qt linguist tools found.")
    message(WARNING "Translations will not be built.")
    add_custom_target(GenerateEmbeddedTranslations)
else()
    add_custom_target(GenerateEmbeddedTranslations)
endif()

# target must be created AFTER Qt cmake was run and automoc enabled
CreateTarget(${CURRENT_TARGET} EXECUTABLE ${CURRENT_TARGET_NAME} 20)
add_dependencies(${CURRENT_TARGET} GenerateEmbeddedTranslations)

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
