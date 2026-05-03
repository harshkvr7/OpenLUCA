#include <drogon/drogon.h>
#include "core/taxonomy_manager.hpp"
#include <iostream>

int main() {
    std::cout << "Booting Taxonomy API...\n";

    // 1. Initialize data via the Singleton instance.
    TaxonomyManager::getInstance().initialize("../data/nodes.dmp", "../data/names.dmp", "taxonomy_cache.bin");

    std::cout << "Data loaded. Loading config and starting server...\n";

    // 2. Load config and start Drogon
    drogon::app().loadConfigFile("../config.json");
    drogon::app().run();

    return 0;
}