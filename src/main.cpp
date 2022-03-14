
#include "emscriptenstuff.h"
#include "settings.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
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

int getIndexFromFilename(std::filesystem::path path) {
    auto filename = path.filename();

    for (auto c : filename.string()) {
        if (c <= '9' && c >= '0') {
            return c - '0';
        }
    }

    std::cerr << "could not find number in filename " << path << "\n";
    std::terminate();
}

std::string getDateString() {
    auto pad = [](int i) {
        if (i < 10) {
            return "0" + std::to_string(i);
        }
        else {
            return std::to_string(i);
        }
    };

    std::stringstream ss;

    auto t = std::time(nullptr);
    auto info = std::localtime(&t);

    ss << 20 << pad(info->tm_year % 100) << "-" << pad(info->tm_mon + 1) << "-"
       << pad(info->tm_mday);
    return ss.str();
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

    auto files = std::vector<int>{};

    if (std::filesystem::is_directory(path)) {
        for (auto &it : std::filesystem::directory_iterator{tmpPath}) {
            auto outPath =
                (path / (getDateString() + " " + getTitle(it.path())))
                    .replace_extension(".txt")
                    .string();
            auto of = std::ofstream{outPath};
            std::cout << it.path() << " -> " << outPath << std::endl;

            convert(it.path(), of);
            if (!std::filesystem::exists(outPath)) {
                std::cerr << "failed to convert file " << outPath << "\n";
                std::terminate();
            }

            files.push_back(getIndexFromFilename(it.path()));
        }
    }
    else {
        convert(path);
    }

    std::filesystem::remove_all(tmpPath);

    if (settings.shouldClear) {
        for (auto i : files) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(5s); // neo needs to restart inbetween
            std::cout << "clearing file " << i << std::endl;
            if (std::system(
                    ("neotools files clear " + std::to_string(i)).c_str())) {
                std::cerr << "failed to clear file " << i << "\n";
                std::terminate();
            }
        }
    }
}

#else

int main(int argc, char **argv) {
    initEmscriptenStuff();
}

#endif
