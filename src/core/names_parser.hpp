#pragma once
#include <string>
#include <string_view>
#include <functional>

// Parses the NCBI names.dmp file, passing string_views to avoid heap copying.
void parseNamesFile(const std::string& filepath, const std::function<void(int, std::string_view, std::string_view)>& on_name_parsed);