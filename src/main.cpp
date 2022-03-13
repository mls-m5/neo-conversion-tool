
#include "emscriptenstuff.h"
#include "settings.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
std::string replaceAll(std::string str,
                       const std::string &from,
                       const std::string &to) {
    for (size_t start_pos = 0;
         (start_pos = str.find(from, start_pos)) != std::string::npos;) {
        str.replace(start_pos, from.length(), to);
        start_pos +=
            to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

auto replacements = std::vector<std::pair<unsigned char, std::string>>{
    {'@', "Å"},
    {'<', "Ä"},
    {'>', "Ö"},

    {'\'', "å"},
    {',', "ä"},
    {'.', "ö"},

    {'\\', "<"},
    {'|', ">"},
    {';', "."},
    {'#', "'"},
    {'~', "*"},

    // { ... } Tilde?
    //    {'\163', "#"}, // £ // multichar
    //    {'\162', "@"}, // ¢ // multichar
    {'(', ")"},
    {'*', "("},
    {'^', "&"},

    {'[', "+"},
    {'{', "?"},
    {'/', ","},
    {'?', ";"},
};

std::string convert(std::string line) {
    auto convert = [](unsigned char c) {
        for (auto &r : replacements) {
            if (r.first == c) {
                return r.second;
            }
        }

        return std::string{static_cast<char>(c)};
    };

    auto ret = std::string{};
    for (auto c : line) {
        ret += convert(c);
    }

    ret = replaceAll(ret, "£", "#");
    ret = replaceAll(ret, "¢", "@");

    return ret;
}

std::string getTitle(std::filesystem::path path) {
    std::string disallowed = "\\/<>|\":?*";

    auto file = std::ifstream{path};
    for (std::string line; std::getline(file, line);) {
        if (!line.empty()) {
            //            auto name = std::string{};
            //            for (auto &c : line) {
            //                name += convert(c);
            //            }
            auto name = convert(line);

            for (auto c : name) {
                if (disallowed.find(c) != std::string::npos) {
                    break;
                }
            }

            if (!name.empty()) {
                return name;
            }
        }
    }

    return path.filename();
}

void convert(std::filesystem::path path, std::ostream &stream = std::cout) {
    std::cout << path << std::endl;

    if (path.extension() != ".txt") {
        return;
    }

    auto file = std::ifstream{path};

    if (!file.is_open()) {
        std::cerr << "could not open file " << path << " \n";
        std::terminate();
    }

    for (std::string line; std::getline(file, line);) {
        stream << convert(line) << "\n";
    }
}

#ifndef __EMSCRIPTEN__

int main(int argc, char **argv) {
    const auto settings = Settings{argc, argv};

    auto tmpPath = std::filesystem::path{"/tmp/neo-char-conv"};
    auto path = settings.outputPath;

    if (path.empty()) {
        std::cout << "please specify existing path with --path\n";
        return 1;
    }

    if (!std::filesystem::is_directory(path)) {
        std::cout << path << "is not a existing directory\n";
        return 1;
    }

    std::filesystem::remove_all(tmpPath);
    std::filesystem::create_directories(tmpPath);

    if (std::system(
            ("neotools files read-all -p " + tmpPath.string()).c_str())) {
        std::cerr << "loading of neo files failed";
        return 1;
    }

    if (std::filesystem::is_directory(path)) {
        for (auto &it : std::filesystem::directory_iterator{tmpPath}) {
            auto outPath =
                (path / getTitle(it.path())).replace_extension(".txt");
            auto of = std::ofstream{outPath};
            convert(it.path(), of);
            if (!std::filesystem::exists(outPath)) {
                std::cerr << "failed to convert file " << outPath << "\n";
                std::terminate();
            }
        }
    }
    else {
        convert(path);
    }

    std::filesystem::remove_all(tmpPath);

    if (settings.shouldClear) {
        return std::system("neotools files clear");
    }
}

#else

int main(int argc, char **argv) {
    initEmscriptenStuff();
}

#endif
