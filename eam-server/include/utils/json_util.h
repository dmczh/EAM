#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>

using json = nlohmann::json;

class JsonHelper
{
public:
    JsonHelper(/* args */) {};
    ~JsonHelper() {};
    // 解析JSON字符串 （？）
    static json parse(const std::string &jsonStr);
    template <typename T>
    static std::string stringify(const T &obj);
    static json successResponse(const std::string &message = "",
                                const json &data = json());
    static json errorResponse(int code, std::string &message);
    static json paginatedResponse(const json &data,
                                  int page,
                                  int pageSize,
                                  int total);
};
