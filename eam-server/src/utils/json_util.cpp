#include "json_util.h"
#include "utils/logger.h"

json JsonHelper::parse(const std::string &jsonStr)
{
    try
    {
        return json::parse(jsonStr);
    }
    catch (const json::parse_error &e)
    {
        EAM_LOG_ERROR("JSON parse error:" + std::string(e.what()));
        return json();
    }
}
template <typename T>
std::string JsonHelper::stringify(const T &obj)
{
    try
    {
        json j = obj;
        return j.dump();
    }
    catch (const std::exception &e)
    {
        EAM_LOG_ERROR("JSON stringify error:" + std::string(e.what()));
        return "{}";
    }
}

json JsonHelper::successResponse(const std::string &message,
                                 const json &data)
{
    json response;
    response["success"] = true;
    response["code"] = 200;
    if (!message.empty())
    {
        response["message"] = message;
    }
    if (!data.is_null())
    {
        response["data"] = data;
    }
    return response;
}

json JsonHelper::errorResponse(int code, std::string &message)
{
    json response;
    response["success"] = false;
    response["code"] = code;
    response["message"] = message;
    return response;
}

json JsonHelper::paginatedResponse(const json &data,
                                   int page,
                                   int pageSize,
                                   int total)
{
    json response = successResponse();
    response["data"] = data;
    response["pagination"] = {
        {"page", page},
        {"page_size", pageSize},
        {"total", total},
        {"total_pages", (total + pageSize - 1) / pageSize}};
    return response;
}