#pragma once

#include <filesystem>
#include <iosfwd>
#include <vector>

void viewHtml(std::vector<std::filesystem::path> path, std::ostream &file);
