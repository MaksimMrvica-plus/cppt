#ifndef _JSONTOOL_H
#define _JSONTOOL_H

#include <iostream>
#include <utility> // std::pair
#include "json.hpp"
#include "../client/define.h"

using json          = nlohmann::json;
using ordered_json  = nlohmann::ordered_json;
using ojson         = nlohmann::ordered_json;

void ModifyOrAdd(json &j, const std::pair<std::string, json> &kv, int overwrite = 1); // 修改json消息 默认覆盖
int SetJsonKV(json &j, const std::pair<std::string, json> &kv);
int SetOrdJsonKV(ordered_json &j, const std::pair<std::string, json> &kv);

json createJsonMessage(                    // 创建组装 JSON 消息
    const std::string &cipher = "",        // 加密信息
    const std::string &type = "",          // 请求或响应的类型
    const std::string &username = "",      // 用户名
    const std::string &password = "",      // 密码
    const json &data = json::object(),     // 业务数据
    const std::string &time = "",          // 时间
    const std::string &timestamp = "",     // 时间戳
    const std::string &status = "",        // 请求或响应的状态
    const std::string &error_code = "",    // 错误代码
    const std::string &message = "",       // 消息内容
    const std::string &error_message = "", // 错误信息
    const std::string &request_id = "",    // 请求ID
    const std::string &token = "",         // 身份验证令牌
    const std::string &version = ""        // 协议版本号
);
ordered_json createOrderedJsonMessage(     // 创建有序 JSON 消息
    const std::string &cipher = "",        // 加密信息
    const std::string &type = "",          // 请求或响应的类型
    const std::string &username = "",      // 用户名
    const std::string &password = "",      // 密码
    const json &data = json::object(),     // 业务数据
    const std::string &time = "",          // 时间
    const std::string &timestamp = "",     // 时间戳
    const std::string &status = "",        // 请求或响应的状态
    const std::string &error_code = "",    // 错误代码
    const std::string &message = "",       // 消息内容
    const std::string &error_message = "", // 错误信息
    const std::string &request_id = "",    // 请求ID
    const std::string &token = "",         // 身份验证令牌
    const std::string &version = ""        // 协议版本号
);

#endif