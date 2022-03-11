
#include "emscriptenstuff.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

auto replacements = std::vector<std::pair<char, std::string>>{
    {',', "ä"},
    {'\'', "å"},
    {'.', "ö"},
    {'/', ","},
    {'{', "?"},
    {';', "."},
    {'(', ")"},
    {'*', "("},
};

std::string convert(char c) {
    for (auto &r : replacements) {
        if (r.first == c) {
            return r.second;
        }
    }
    return std::string{c};
}

void convert(std::filesystem::path path, std::ostream &stream = std::cout) {
    std::cout << path << std::endl;

    if (path.extension() != ".txt") {
        return;
    }

    auto file = std::ifstream{path};

    if (!file.is_open()) {
        std::cerr << "could not open file " << path << " \n";
        std::exit(1);
    }

    for (std::string line; std::getline(file, line);) {
        for (auto c : line) {
            stream << convert(c);
        }
        stream << "\n";
    }
}

#ifndef __EMSCRIPTEN__

int main(int argc, char **argv) {
    auto args = std::vector<std::string>{argv, argv + argc};

    if (args.size() == 1) {
        std::cerr << "please specify file to convert\n";
        return 1;
    }

    auto path = std::filesystem::path{args.at(1)};

    if (std::filesystem::is_directory(path)) {
        for (auto it : std::filesystem::directory_iterator{path}) {
            auto of = std::ofstream{it.path().string() + ".conv"};
            convert(it.path(), of);
        }
    }
    else {
        convert(path);
    }
}

#else

int main(int argc, char **argv) {
    initEmscriptenStuff();
}

#endif
