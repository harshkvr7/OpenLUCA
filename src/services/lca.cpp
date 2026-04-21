#include "lca.hpp"
#include "lineage.hpp"

int findLCANaive(const TaxonomyManager& manager, int tax_id1, int tax_id2) {
    std::vector<int> path1 = getLineage(manager, tax_id1);
    std::vector<int> path2 = getLineage(manager, tax_id2);

    int lca = 1; 
    size_t min_depth = std::min(path1.size(), path2.size());
    for (size_t i = 0; i < min_depth; ++i) {
        if (path1[i] == path2[i]) lca = path1[i];
        else break; 
    }
    return lca;
}

int findLCABinaryLifting(const TaxonomyManager& manager, int u, int v) {
    // Boundary checks for the flat arrays
    if (u < 0 || u >= manager.max_nodes || v < 0 || v >= manager.max_nodes) return 1;

    if (manager.depth_arr[u] < manager.depth_arr[v]) std::swap(u, v);

    // 1. Equalize depths (Matrix indexing math: Node * LOG + offset)
    for (int i = manager.LOG - 1; i >= 0; i--) {
        if (manager.depth_arr[u] - (1 << i) >= manager.depth_arr[v]) {
            u = manager.up_arr[u * manager.LOG + i];
        }
    }

    if (u == v) return u;

    // 2. Simultaneous Jump
    for (int i = manager.LOG - 1; i >= 0; i--) {
        if (manager.up_arr[u * manager.LOG + i] != manager.up_arr[v * manager.LOG + i]) {
            u = manager.up_arr[u * manager.LOG + i];
            v = manager.up_arr[v * manager.LOG + i];
        }
    }

    // 3. Parent is LCA
    return manager.up_arr[u * manager.LOG + 0];
}