#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

inline const auto helpText = R"_(
usage

--path    -p        where to put converted files
--clear             remove files from device after transfer
)_";

struct Settings {
    std::filesystem::path outputPath;
    bool shouldClear = false;

    [[noreturn]] static void printHelp() {
        std::cout << helpText << std::endl;
        std::exit(0);
    }

    Settings(int argc, char **argv) {
        const auto args = std::vector<std::string>(argv, argv + argc);

        if (args.size() <= 1) {
            printHelp();
        }
        for (size_t i = 1; i < args.size(); ++i) {
            auto getNextArg = [&args, &i] {
                ++i;
                if (i >= args.size()) {
                    std::cerr << "more arguments for action\n";
                    std::terminate();
                }

                return args.at(i);
            };

            auto arg = args.at(i);

            if (arg == "-p" || arg == "--path") {
                outputPath = args.at(++i);
            }
            else if (arg == "--clear") {
                shouldClear = true;
            }
        }
    }
};
