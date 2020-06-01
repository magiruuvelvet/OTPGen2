#include <iostream>

#include <bandit/bandit.h>

#include "core_tests/otptoken_tests.hpp"
#include "core_tests/tokenstore_tests.hpp"
#include "core_tests/qr_tests.hpp"

int main(int argc, char **argv)
{
    return bandit::run(argc, argv);
}
