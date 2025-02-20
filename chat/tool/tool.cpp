#include "tool.h"


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