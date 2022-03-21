#pragma once

#include <string>

// https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
inline std::string replaceAll(std::string str,
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
