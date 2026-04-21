#pragma once
#include <vector>
#include <string>

class TaxonomyManager {
public:
    static TaxonomyManager& getInstance() {
        static TaxonomyManager instance;
        return instance;
    }

    TaxonomyManager(const TaxonomyManager&) = delete;
    TaxonomyManager& operator=(const TaxonomyManager&) = delete;

    // --- FLAT ARRAYS (O(1) Locality) ---
    int max_nodes = 0;
    static constexpr int LOG = 20;

    std::vector<int> parent_arr;
    std::vector<std::string> name_arr;
    std::vector<int> depth_arr;
    std::vector<int> up_arr; // Flattened 2D Matrix: index = [u * LOG + i]

    void initialize(const std::string& nodes_file, const std::string& names_file, const std::string& cache_file);
    std::string getName(int tax_id) const;

private:
    TaxonomyManager() = default; 
    bool loadCache(const std::string& cache_file);
    void saveCache(const std::string& cache_file) const;
    void buildFromDumps(const std::string& nodes_file, const std::string& names_file);
    
    // DFS with zero-allocation adjacency lists
    void buildBinaryLiftingTables();
    void dfs(int u, int p, int d, const std::vector<int>& head, const std::vector<int>& to, const std::vector<int>& next);
};