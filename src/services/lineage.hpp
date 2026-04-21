#pragma once
#include <vector>
#include "../core/taxonomy_manager.hpp"

// Computes the ancestral path from the root to the target species
std::vector<int> getLineage(const TaxonomyManager& manager, int tax_id);

void printLineage(const TaxonomyManager& manager, int tax_id);