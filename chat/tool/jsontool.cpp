#include "jsontool.h"




// 函数：修改或添加键值对
void ModifyOrAdd(json &j, const std::pair<std::string, json> &kv, int overwrite)
{
    const std::string &key = kv.first;
    const json &value = kv.second;

    // 检查键是否存在
    if (j.contains(key))
    {
        if (overwrite == 1)
        {
            // 键存在且允许覆盖，修改值
            j[key] = value;
            std::cout << "Modified existing key: " << key << std::endl;
        }
        else
        {
            // 键存在但不允许覆盖，跳过
            std::cout << "Key already exists and overwrite is disabled. Skipping: " << key << std::endl;
        }
    }
    else
    {
        // 键不存在，添加键值对
        j[key] = value;
        std::cout << "Added new key: " << key << std::endl;
    }
}

// 函数：修改键值对
int SetJsonKV(json &j, const std::pair<std::string, json> &kv)
{
    const std::string &key = kv.first;
    const json &value = kv.second;
    // 检查键是否存在
    if (j.contains(key))
    {
        j[key] = value;
        return SUCCESS;
    }else{
        std::cerr << "Key not exists: [" << key << ']' << std::endl;
        return FAILURE;
    }
    return FAILURE;
}

// 函数：修改键值对
int SetOrdJsonKV(ordered_json &j, const std::pair<std::string, ordered_json> &kv)
{
    const std::string &key = kv.first;
    const ordered_json &value = kv.second;
    // 检查键是否存在
    if (j.contains(key))
    {
        j[key] = value;
        return SUCCESS;
    }else{
        std::cerr << "Key not exists: [" << key << ']' << std::endl;
        return FAILURE;
    }
    return FAILURE;
}

json createJsonMessage(const std::string &cipher, const std::string &type, const std::string &username, const std::string &password,
                       const json &data, const std::string &time, const std::string &timestamp, const std::string &status, const std::string &error_code,
                       const std::string &message, const std::string &error_message, const std::string &request_id, const std::string &token, const std::string &version)
{
    json j;
    j["cipher"] = cipher;               // 加密信息
    j["type"] = type;                   // 请求或响应的类型
    j["username"] = username;           // 用户名
    j["password"] = password;           // 密码
    j["message"] = message;             // 消息内容
    j["time"] = time;                   // 时间
    j["status"] = status;               // 请求或响应的状态
    j["error_code"] = error_code;       // 错误代码
    j["error_message"] = error_message; // 错误信息
    j["request_id"] = request_id;       // 请求ID
    j["timestamp"] = timestamp;         // 时间戳
    j["data"] = data;                   // 业务数据
    j["token"] = token;                 // 身份验证令牌
    j["version"] = version;             // 协议版本号
    return j;
}

ordered_json createSystemOrdJsonMessage(const std::string &cipher, const std::string &type, const std::string &username, const std::string &password,
                       const ordered_json &data, const std::string &time, const std::string &timestamp, const std::string &status, const std::string &error_code,
                       const std::string &message, const std::string &error_message, const std::string &request_id, const std::string &token, const std::string &version)
{
    ordered_json j;
    j["cipher"] = cipher;               // 加密信息
    j["type"] = type;                   // 请求或响应的类型
    j["username"] = username;           // 用户名
    j["password"] = password;           // 密码
    j["data"] = data;                   // 业务数据
    j["time"] = time;                   // 时间
    j["timestamp"] = timestamp;         // 时间戳
    j["status"] = status;               // 请求或响应的状态
    j["error_code"] = error_code;       // 错误代码
    j["message"] = message;             // 消息内容
    j["error_message"] = error_message; // 错误信息
    j["request_id"] = request_id;       // 请求ID
    j["token"] = token;                 // 身份验证令牌
    j["version"] = version;             // 协议版本号
    return j;
}



json parseJsonMessage(const std::string& jsonString)
{
    try
    {
        return json::parse(jsonString);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return json(); // Return an empty JSON object on failure
    }
}

// 创建发送消息内容 json 数据
ordered_json createSendMessageJson(
    const std::string &messageId,
    const std::string &sender,
    const std::string &receiver,
    const std::string &type,
    const ordered_json &content,
    const uint64_t timestamp, // 使用整数类型的时间戳
    const std::string &status,
    const uint64_t expireAt, // 使用整数类型的过期时间
    const ordered_json &spare
){
    ordered_json j;
    j["messageId"] = messageId;
    j["sender"] = sender;
    j["receiver"] = receiver;
    j["type"] = type;
    j["content"] = content;
    j["timestamp"] = timestamp;
    j["status"] = status;
    j["expireAt"] = expireAt;
    j["spare"] = spare;
    return j;
}

// 创建发送消息内容 json 数据
ordered_json createSendPerMsgJson(
    const std::string &messageId,
    const std::string &sender,
    const std::string &receiver,
    const std::string &type,
    const ordered_json &content,
    const uint64_t timestamp, // 使用整数类型的时间戳
    const std::string &status,
    const uint64_t expireAt, // 使用整数类型的过期时间
    const ordered_json &spare
){
    ordered_json j;
    j["messageId"] = messageId;
    j["sender"] = sender;
    j["receiver"] = receiver;
    j["type"] = type;
    j["content"] = content;
    j["timestamp"] = timestamp;
    j["status"] = status;
    j["expireAt"] = expireAt;
    j["spare"] = spare;
    return j;
}

ordered_json createUserProfileJson(
    const std::string &nickname,
    const std::string &gender,
    const std::string &birthday,
    const std::string &bio,
    const std::string &location,
    const std::string &occupation,
    const std::string &interests,
    const std::string &education,
    const std::string &website
) {
    ordered_json j;
    j["nickname"] = nickname;
    j["gender"] = gender;
    j["birthday"] = birthday;
    j["bio"] = bio;
    j["location"] = location;
    j["occupation"] = occupation;
    j["interests"] = interests;
    j["education"] = education;
    j["website"] = website;
    return j;
}

// 函数定义
void ParseAndPrintContentArray(const ordered_json &content)
{
    if (!content.is_array())
    {
        std::cerr << "ERROR | content is not a JSON array" << '\n';
        return;
    }

    for (size_t i = 0; i < content.size(); ++i)
    {
        const ordered_json &item = content[i];

        if (!item.is_object())
        {
            std::cerr << "ERROR | Item at index " << i << " is not a JSON object" << '\n';
            continue;
        }

        std::string type = item.value("type", "N/A");
        std::string sub_type = item.value("sub_type", "N/A");
        std::string data = item.value("data", "N/A");

        std::cout << "--------------------------\n"
                  << "Message " << (i + 1) << ":\n"
                  << "  Type: " << type << '\n'
                  << "  Sub-Type: " << sub_type << '\n'
                  << "  Data: " << data << '\n'
                  << "--------------------------\n";
    }
}
