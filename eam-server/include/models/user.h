// model/user.h
#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "utils/json_util.h"
#include "nlohmann/json.hpp"
#include "utils/logger.h"

namespace model
{

    struct Role
    {
        int64_t id = 0;
        std::string code;
        std::string name;
        std::string description;
        int data_scope = 1;
        int status = 1;
    };

    struct Permission
    {
        int64_t id = 0;
        int64_t parent_id = 0;
        int type = 1; // 1菜单 2按钮 3接口
        std::string name;
        std::string code;
        std::string path;
        std::string component;
        std::string icon;
        int sort_order = 0;
        int status = 1;

        std::vector<Permission> children;
    };

    struct User
    {
        int64_t id = 0;
        std::string username;
        std::string passwordHash;
        std::string email;
        std::string phone;
        bool account_non_expired;
        bool account_non_locked;
        bool enabled;
        time_t created_at;
        time_t updated_at;
        time_t last_login_at;
        int login_fail_count = 0;

        // 关联数据
        std::vector<Role> roles;
        std::vector<Permission> permissions;
    };

    struct UserProfiles
    {
        int64_t id = 0;
        std::string role_key = "";
        std::string role_name = "";
        std::string discription = "";
        int16_t data_scope;
        int64_t created_by;
        std::chrono::system_clock::time_point created_at;
    };

    // 资产相关模型
    struct Asset
    {
        int64_t id = 0;
        std::string asset_code;
        std::string asset_name;
        int64_t category_id = 0;
        std::string specification;
        std::string unit;
        std::string purchase_date;
        double purchase_price = 0.0;
        std::string supplier;
        std::string brand;
        std::string model;
        int64_t department_id = 0;
        int64_t keeper_id = 0;
        std::string location;
        int status = 1;
        std::string qr_code;
        std::string remark;
        int64_t created_by = 0;

        // 关联字段
        std::string category_name;
        std::string keeper_name;
        std::string department_name;

        // 计算字段
        double net_value = 0.0;
        double accumulated_depreciation = 0.0;
    };

    struct DepreciationRecord
    {
        int64_t id = 0;
        int64_t asset_id = 0;
        std::string depreciate_date;
        std::string period; // YYYY-MM
        double amount = 0.0;
        double accumulated = 0.0;
        double net_value = 0.0;
    };

} // namespace model