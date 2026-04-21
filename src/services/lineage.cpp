#include "lineage.hpp"
#include <iostream>
#include <algorithm>

std::vector<int> getLineage(const TaxonomyManager& manager, int tax_id) {
    std::vector<int> lineage;
    int current = tax_id;

    while (current != 1 && current != 0) {
        lineage.push_back(current);
        
        // Array Boundary Guard
        if (current < 0 || current >= manager.max_nodes) break;
        
        int parent = manager.parent_arr[current];
        if (parent != 0 && parent != current) {
            current = parent;
        } else {
            break; 
        }
    }
    if (current == 1) lineage.push_back(1);
    
    std::reverse(lineage.begin(), lineage.end());
    return lineage;
}

void printLineage(const TaxonomyManager& manager, int tax_id) {
    std::vector<int> lineage = getLineage(manager, tax_id);
    for (size_t i = 0; i < lineage.size(); ++i) {
        for (size_t j = 0; j < i; ++j) std::cout << "  ";
        std::cout << "-> " << manager.getName(lineage[i]) << " (" << lineage[i] << ")\n";
    }
}