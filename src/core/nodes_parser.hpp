#pragma once
#include <string>
#include <functional>

// Parses the NCBI nodes.dmp file and streams data via a callback to avoid large memory allocations.
void parseNodesFile(const std::string& filepath, const std::function<void(int, int)>& on_node_parsed);