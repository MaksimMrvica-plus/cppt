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
int SetOrdJsonKV(ordered_json &j, const std::pair<std::string, ordered_json> &kv);

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
ordered_json createSystemOrdJsonMessage(     // 创建有序 JSON 消息
    const std::string &cipher = "",        // 加密信息
    const std::string &type = "",          // 请求或响应的类型
    const std::string &username = "",      // 用户名
    const std::string &password = "",      // 密码
    const ordered_json &data = ordered_json::object(),     // 业务数据
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

// 创建发送消息内容 json 数据
ordered_json createSendMessageJson(
    const std::string &messageId = "",
    const uint64_t senderId = 0,
    const uint64_t receiverId = 0,
    const std::string &type ="",
    const ordered_json &content = ordered_json::object(),
    const uint64_t timestamp = 0, // 使用整数类型的时间戳
    const std::string &status = "",
    const uint64_t expireAt = 0, // 使用整数类型的过期时间
    const ordered_json &spare = ordered_json::object()
);

// 创建发送给 个人的 消息内容 json 数据
ordered_json createSendPerMsgJson(
    const std::string &messageId = "",
    const uint64_t senderId = 0,
    const uint64_t receiverId = 0,
    const std::string &type ="",
    const ordered_json &content = ordered_json::object(),
    const uint64_t timestamp = 0, // 使用整数类型的时间戳
    const std::string &status = "",
    const uint64_t expireAt = 0, // 使用整数类型的过期时间
    const ordered_json &spare = ordered_json::object()
);

// 个人资料格式
ordered_json createUserProfileJson(
    const std::string &nickname = "",
    const std::string &gender = "",
    const std::string &birthday = "",
    const std::string &bio = "",
    const std::string &location = "",
    const std::string &occupation = "",
    const std::string &interests = "",
    const std::string &education = "",
    const std::string &website = ""
);

// 创建发送给 群组的 消息内容 json 数据
ordered_json createSendGrpMsgJson();
#endif