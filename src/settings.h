#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

inline const auto helpText = R"_(
usage

--path    -p        where to put converted files
--clear             remove files from device after transfer
--view              shows result in a web-browser
)_";

struct Settings {
    std::filesystem::path outputPath;

    std::filesystem::path tmpPath{"/tmp/neo-char-conv"};
    bool shouldClear = false;
    bool shouldView = false;
    bool shouldRemoveOutputPath = false;

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
            else if (arg == "--view") {
                shouldView = true;
            }
        }

        if (shouldView) {
            if (outputPath.empty()) {
                outputPath = "/tmp/neo-conv-output/";
                std::filesystem::remove_all(outputPath);
                std::filesystem::create_directories(outputPath);
                shouldRemoveOutputPath = true;
            }
        }

        if (outputPath.empty()) {
            std::cout << "please specify existing path with --path\n";
            std::terminate();
        }

        if (!std::filesystem::is_directory(outputPath)) {
            std::cout << outputPath << "is not a existing directory\n";
            std::terminate();
        }

        std::filesystem::remove_all(tmpPath);
        std::filesystem::create_directories(tmpPath);
    }

    ~Settings() {
        if (shouldRemoveOutputPath && !outputPath.empty()) {
            std::cout << "removing temporary " << outputPath << std::endl;
            std::filesystem::remove_all(outputPath);
        }
    }
};
