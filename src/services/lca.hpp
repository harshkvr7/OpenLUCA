#pragma once
#include <vector>
#include "core/taxonomy_manager.hpp"

int findLCANaive(const TaxonomyManager& manager, int tax_id1, int tax_id2);
int findLCABinaryLifting(const TaxonomyManager& manager, int tax_id1, int tax_id2);