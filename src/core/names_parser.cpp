#include "names_parser.hpp"
#include <fstream>

static int fast_atoi(std::string_view s) {
    int val = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') val = val * 10 + (c - '0');
    }
    return val;
}

void parseNamesFile(const std::string& filepath, const std::function<void(int, std::string_view, std::string_view)>& on_name_parsed) {
    std::ifstream file(filepath);
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::string_view sv(line);
        
        // Column 1: tax_id
        size_t p1 = sv.find("\t|\t");
        if (p1 == std::string_view::npos) continue;
        
        // Column 2: name_txt (The actual name string)
        size_t p2 = sv.find("\t|\t", p1 + 3);
        if (p2 == std::string_view::npos) continue;
        
        // Column 3: unique_name 
        size_t p3 = sv.find("\t|\t", p2 + 3);
        if (p3 == std::string_view::npos) continue;

        // Column 4: name_class (Used to filter for "scientific name")
        // Note: The final column terminates with "\t|" not "\t|\t"
        size_t p4 = sv.find("\t|", p3 + 3);

        int tax_id = fast_atoi(sv.substr(0, p1));
        std::string_view name = sv.substr(p1 + 3, p2 - p1 - 3);
        std::string_view name_class = sv.substr(p3 + 3, p4 - p3 - 3);

        on_name_parsed(tax_id, name, name_class);
    }
}