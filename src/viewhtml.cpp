#include "viewhtml.h"
#include <fstream>
#include <iostream>

namespace {

auto html = R"_(
<html>
<body>

)_";

auto htmlFooter = R"_(
</body>
</html>
)_";

} // namespace

void viewHtml(std::vector<std::filesystem::path> paths, std::ostream &file) {
    file << html;

    for (auto &path : paths) {
        file << "<h2>" << path.string() << "</h2>\n";
        auto ifile = std::ifstream{path};
        for (std::string line; getline(ifile, line);) {
            file << "<p>" << line << "</p>\n";
        }
        file << "<br>----<br>";
    }

    file << htmlFooter << std::endl;
}
