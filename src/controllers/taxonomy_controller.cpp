#include "taxonomy_controller.hpp"
#include "core/taxonomy_manager.hpp"
#include "services/lineage.hpp"
#include "services/lca.hpp"
#include <chrono> 

void TaxonomyController::getLineage(const drogon::HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                    int tax_id) {
    
    const auto& manager = TaxonomyManager::getInstance();

    // --- start timer ---
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<int> path = ::getLineage(manager, tax_id);
    
    // --- stop timer ---
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> algo_time = end - start;

    if (path.empty()) {
        Json::Value error;
        error["error"] = "Tax ID not found or has no lineage";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
        return;
    }

    Json::Value ret;
    Json::Value lineage_array(Json::arrayValue);

    for (int id : path) {
        Json::Value node;
        node["tax_id"] = id;
        node["name"] = manager.getName(id);
        lineage_array.append(node);
    }

    ret["algorithm_latency_ms"] = algo_time.count();
    ret["tax_id"] = tax_id;
    ret["lineage"] = lineage_array;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void TaxonomyController::getLca(const drogon::HttpRequestPtr &req,
                                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                int tax_id1, int tax_id2) {
    
    const auto& manager = TaxonomyManager::getInstance();
    std::string name1 = manager.getName(tax_id1);
    std::string name2 = manager.getName(tax_id2);

    if (name1 == "Unknown" || name2 == "Unknown") {
        Json::Value error;
        error["error"] = "One or both Tax IDs not found in the database";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
        return;
    }

    std::string algo = req->getParameter("algorithm");
    int lca_id = 1;

    // --- start timer ---
    auto start = std::chrono::high_resolution_clock::now();

    if (algo == "naive") {
        lca_id = ::findLCANaive(manager, tax_id1, tax_id2);
    } else {
        lca_id = ::findLCABinaryLifting(manager, tax_id1, tax_id2);
        algo = "binary_lifting";
    }

    // --- stop timer ---
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> algo_time = end - start;

    Json::Value ret;
    
    ret["algorithm_latency_ms"] = algo_time.count();
    ret["algorithm_used"] = algo;
    ret["species_1"] = name1;
    ret["tax_id_1"] = tax_id1;
    ret["species_2"] = name2;
    ret["tax_id_2"] = tax_id2;
    
    Json::Value lca_node;
    lca_node["tax_id"] = lca_id;
    lca_node["name"] = manager.getName(lca_id);
    ret["lca"] = lca_node;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}