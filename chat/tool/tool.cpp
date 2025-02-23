#include "tool.h"
#include <rpc.h>
#pragma comment(lib,"rpcrt4.lib")

// 获取当前时间戳
uint64_t getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    return static_cast<uint64_t>(now);
}

// 将时间戳转换为字符串格式
std::string timestampToString(uint64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}


// 将字符串格式转换为时间戳
uint64_t stringToTimestamp(const std::string& datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::time_t time = std::mktime(&tm);
    return static_cast<uint64_t>(time);
}


// 生成 string 字符串格式 UUID
std::string generate_uuid() {
    
    UUID uuid;
    UuidCreate(&uuid);  // 生成 UUID

    RPC_CSTR uuid_str;
    UuidToStringA(&uuid, &uuid_str);  // 转换为字符串

    std::string result(reinterpret_cast<char*>(uuid_str));
    RpcStringFreeA(&uuid_str);  // 释放内存

    return result;
}

// 解析函数 默认解析 4 个 []
std::vector<std::string> ParseMessageCust(const std::string& message, int num, char openChar, char closeChar) {
    std::vector<std::string> result;
    size_t pos = 0;

    for (int i = 0; i < num; ++i) {
        // 查找开始字符
        pos = message.find(openChar, pos);
        if (pos == std::string::npos) {
            break; // 如果找不到开始字符，退出循环
        }

        // 查找结束字符
        size_t end = message.find(closeChar, pos);
        if (end == std::string::npos) {
            break; // 如果找不到结束字符，退出循环
        }

        // 提取内容
        result.push_back(message.substr(pos + 1, end - pos - 1));
        pos = end + 1;
    }

    return result;
}


// 解析函数
std::vector<std::string> ParseMessage4(const std::string& message) {
    std::vector<std::string> result;
    size_t pos = 0;

    // 解析第一个 []
    pos = message.find("[", pos);
    if (pos != std::string::npos) {
        size_t end = message.find("]", pos);
        if (end != std::string::npos) {
            result.push_back(message.substr(pos + 1, end - pos - 1));
            pos = end + 1;
        }
    }

    // 解析第二个 []
    pos = message.find("[", pos);
    if (pos != std::string::npos) {
        size_t end = message.find("]", pos);
        if (end != std::string::npos) {
            result.push_back(message.substr(pos + 1, end - pos - 1));
            pos = end + 1;
        }
    }

    pos = message.find("[", pos);
    if (pos != std::string::npos) {
        size_t end = message.find("]", pos);
        if (end != std::string::npos) {
            result.push_back(message.substr(pos + 1, end - pos - 1));
            pos = end + 1;
        }
    }

    pos = message.find("[", pos);
    if (pos != std::string::npos) {
        size_t end = message.find("]", pos);
        if (end != std::string::npos) {
            result.push_back(message.substr(pos + 1, end - pos - 1));
            pos = end + 1;
        }
    }

    return result;
}