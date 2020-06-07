#include <iostream>
#include <vector>

#include <bandit/bandit.h>
#include <fmt/printf.h>

#include "core_tests/otptoken_tests.hpp"
#include "core_tests/tokenstore_tests.hpp"
#include "core_tests/qr_tests.hpp"

bool check_has_info_reporter(const std::vector<const char*> &args)
{
    for (auto&& arg : args)
    {
        if (std::strcmp(arg, "--reporter=info") == 0)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    const std::vector<const char*> args(argv, argc + argv);
    const auto has_info_reporter = check_has_info_reporter(args);
    auto *logger = dynamic_cast<bandit::detail::default_benchmark_logger*>(bandit::detail::benchmark::registered_logger());

    if (!has_info_reporter)
    {
        logger->enable_printing(false);
    }

    const auto res = bandit::run(argc, argv);

    fmt::print("Total time: {:.5f} ms\n", logger->total_time());
    return res;
}
