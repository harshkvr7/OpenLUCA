#pragma once
#include <drogon/HttpController.h>

class TaxonomyController : public drogon::HttpController<TaxonomyController> {
public:
    // Route mappings. The {1} and {2} placeholders inject the URL segments 
    // directly into the method's integer arguments.
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(TaxonomyController::getLineage, "/api/v1/lineage/{1}", drogon::Get);
        ADD_METHOD_TO(TaxonomyController::getLca, "/api/v1/lca/{1}/{2}", drogon::Get);
    METHOD_LIST_END

    void getLineage(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                    int tax_id);

    void getLca(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                int tax_id1, int tax_id2);
};