#include "viewhtml.h"
#include "stringutil.h"
#include <fstream>
#include <iostream>

namespace {

auto html = R"_(
<html>
<body>

)_";

auto htmlFooter = R"_(
<script>
function copy(name) {
    div = document.getElementById(name);
    navigator.clipboard.writeText(div.innerText);
    console.log("copied", name);
}
</script>
</body>
</html>
)_";

} // namespace

void viewHtml(std::vector<std::filesystem::path> paths, std::ostream &file) {
    file << html;

    for (auto &path : paths) {
        file << "<h2>" << path.string() << "</h2>\n";
        file << "<button onclick=\"copy('" << path.string()
             << "')\">copy</button>\n";
        file << "<div id=\"" << path.string() << "\">";
        auto ifile = std::ifstream{path};
        for (std::string line; getline(ifile, line);) {
            line = replaceAll(line, "<", "&lt;");
            line = replaceAll(line, ">", "&gt;");
            file << "<p>" << line << "</p>\n";
        }
        file << "</div>";
    }

    file << htmlFooter << std::endl;
}
