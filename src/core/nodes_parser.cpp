#include "nodes_parser.hpp"
#include <fstream>
#include <string_view>

static int fast_atoi(std::string_view s) {
    int val = 0;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            val = val * 10 + (c - '0');
        }
    }
    return val;
}

void parseNodesFile(const std::string& filepath, const std::function<void(int, int)>& on_node_parsed) {
    std::ifstream file(filepath);
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::string_view sv(line);
        
        // NCBI .dmp format uses "\t|\t" as field separators.
        // We only need the first two columns

        // pos1 marks the end of Column 1 (tax_id)
        size_t pos1 = sv.find("\t|\t");
        if (pos1 == std::string_view::npos) continue;
        
        // pos2 marks the end of Column 2 (parent_tax_id)
        size_t pos2 = sv.find("\t|\t", pos1 + 3);
        if (pos2 == std::string_view::npos) continue;

        // Extract substrings instantly using string_view offsets
        int tax_id = fast_atoi(sv.substr(0, pos1));
        int parent_tax_id = fast_atoi(sv.substr(pos1 + 3, pos2 - pos1 - 3));

        on_node_parsed(tax_id, parent_tax_id);
    }
}