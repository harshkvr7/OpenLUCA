#include "taxonomy_manager.hpp"
#include "nodes_parser.hpp"
#include "names_parser.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

std::string TaxonomyManager::getName(int tax_id) const {
    if (tax_id < 0 || tax_id >= max_nodes || name_arr[tax_id].empty()) {
        return "Unknown";
    }
    return name_arr[tax_id];
}

void TaxonomyManager::initialize(const std::string& nodes_file, const std::string& names_file, const std::string& cache_file) {
    if (!loadCache(cache_file)) {
        std::cout << "Cache not found. Building flat arrays from .dmp files...\n";
        buildFromDumps(nodes_file, names_file);
        
        std::cout << "Precomputing Binary Lifting tables...\n";
        buildBinaryLiftingTables();
        
        std::cout << "Dumping contiguous memory blocks to disk...\n";
        saveCache(cache_file);
    } else {
        std::cout << "Loaded 1D flat cache instantly from disk!\n";
    }
}

void TaxonomyManager::buildFromDumps(const std::string& nodes_file, const std::string& names_file) {
    // 1. Pass: Find max_nodes and populate parent_arr
    parseNodesFile(nodes_file, [this](int u, int p) {
        int needed = std::max(u, p) + 1;
        if (needed > max_nodes) {
            max_nodes = needed;
            parent_arr.resize(max_nodes, 0);
        }
        parent_arr[u] = p;
    });

    // 2. Pass: Sync name_arr size and populate
    name_arr.resize(max_nodes, "");
    parseNamesFile(names_file, [this](int u, std::string_view name, std::string_view name_class) {
        if (name_class == "scientific name") {
            if (u >= max_nodes) {
                max_nodes = u + 1;
                parent_arr.resize(max_nodes, 0);
                name_arr.resize(max_nodes, "");
            }
            name_arr[u] = std::string(name);
        }
    });
}

void TaxonomyManager::dfs(int u, int p, int d, const std::vector<int>& head, const std::vector<int>& to, const std::vector<int>& next) {
    depth_arr[u] = d;
    
    // Set 2^0 ancestor
    up_arr[u * LOG + 0] = p;
    
    // DP for 2^i ancestors using flat math
    for (int i = 1; i < LOG; i++) {
        int prev_ancestor = up_arr[u * LOG + (i - 1)];
        up_arr[u * LOG + i] = up_arr[prev_ancestor * LOG + (i - 1)];
    }
    
    // Traverse children
    for (int e = head[u]; e != -1; e = next[e]) {
        int v = to[e];
        if (v != p) {
            dfs(v, u, d + 1, head, to, next);
        }
    }
}

void TaxonomyManager::buildBinaryLiftingTables() {
    depth_arr.assign(max_nodes, 0);
    up_arr.assign(max_nodes * LOG, 1); // 1 = root

    // Zero-allocation graph structure
    std::vector<int> head(max_nodes, -1);
    std::vector<int> to; 
    std::vector<int> next; 
    to.reserve(max_nodes);
    next.reserve(max_nodes);

    for (int u = 1; u < max_nodes; ++u) {
        int p = parent_arr[u];
        if (p != 0 && p != u) {
            to.push_back(u);
            next.push_back(head[p]);
            head[p] = to.size() - 1;
        }
    }
    
    dfs(1, 1, 0, head, to, next);
}

// --- Lightning Fast Binary I/O via Block Reads ---

bool TaxonomyManager::loadCache(const std::string& cache_file) {
    std::ifstream in(cache_file, std::ios::binary);
    if (!in) return false;

    in.read(reinterpret_cast<char*>(&max_nodes), sizeof(int));

    parent_arr.resize(max_nodes);
    in.read(reinterpret_cast<char*>(parent_arr.data()), max_nodes * sizeof(int));

    depth_arr.resize(max_nodes);
    in.read(reinterpret_cast<char*>(depth_arr.data()), max_nodes * sizeof(int));

    up_arr.resize(max_nodes * LOG);
    in.read(reinterpret_cast<char*>(up_arr.data()), max_nodes * LOG * sizeof(int));

    name_arr.assign(max_nodes, "");
    int u;
    while (in.read(reinterpret_cast<char*>(&u), sizeof(int)) && u != -1) {
        size_t len;
        in.read(reinterpret_cast<char*>(&len), sizeof(size_t));
        std::string s(len, '\0');
        in.read(&s[0], len);
        name_arr[u] = std::move(s);
    }

    return true;
}

void TaxonomyManager::saveCache(const std::string& cache_file) const {
    std::ofstream out(cache_file, std::ios::binary);
    if (!out) return;

    out.write(reinterpret_cast<const char*>(&max_nodes), sizeof(int));
    
    // Blast contiguous memory directly to disk
    out.write(reinterpret_cast<const char*>(parent_arr.data()), max_nodes * sizeof(int));
    out.write(reinterpret_cast<const char*>(depth_arr.data()), max_nodes * sizeof(int));
    out.write(reinterpret_cast<const char*>(up_arr.data()), max_nodes * LOG * sizeof(int));

    // Store sparse string map
    for (int u = 0; u < max_nodes; ++u) {
        if (!name_arr[u].empty()) {
            out.write(reinterpret_cast<const char*>(&u), sizeof(int));
            size_t len = name_arr[u].size();
            out.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
            out.write(name_arr[u].data(), len);
        }
    }
    
    // EOF marker for strings
    int eof = -1;
    out.write(reinterpret_cast<const char*>(&eof), sizeof(int));
}