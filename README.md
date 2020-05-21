# OTPGen 2

Multi-purpose OTP token generator written in C++ and Qt with encryption and System Tray Icon. Supports TOTP, HOTP, Authy and Steam and custom digit lengths and periods.

> This is the repository for the version 2 rewrite.
> Version 1 can be found here: https://github.com/magiruuvelvet/OTPGen

## Cloning the repository

This repository uses git submodules! Make sure to perform an **recursive clone** to avoid issues.

## CMake Build Options

 - `-DENABLE_TESTING` (default *OFF*): build the unit tests
 - `-DBUILD_TRANSLATIONS` (default *OFF*): enables building of translations
 - `-DBUNDLED_CRYPTOPP` (default *OFF*): use the bundled crypto++ library instead of the system shared one
 - `-DBUNDLED_LIBFMT` (default *ON*): use the bundled libfmt instead of the system shared one
 - `-DBUNDLED_CEREAL` (default *ON*): use the bundled cereal header-only library instead of the system-wide copy

## Build Dependencies

 - C++20 compiler **required**
 - CMake 3.14 or higher **required** (I won't support older versions)
 - pkg-config **optimal** (to find some dependencies based on pkg-config)
 - [cereal](https://github.com/USCiLab/cereal) **optimal** (this header-only library is bundled by default)
 - Qt Linguist Tools **optimal** (to build the translations)

## Runtime Dependencies

Most runtime dependencies are also bundled. CMake will prefer shared versions of dependencies
when it finds them and are compatible. Otherwise it falls back to the bundled versions.
Take a close look at the CMake configure output to get an insight what libraries are being used.

 - `Qt` 5.12 or higher (*shared-only*)
 - `crypto++` 8.2+ (*bundled or shared*)
 - `libfmt` 6.2+ (*bundled or shared*)
