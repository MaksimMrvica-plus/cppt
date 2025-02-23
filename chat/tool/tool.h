#ifndef _TOOL_H
#define _TOOL_H

#include <ctime>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <windows.h>

#include "json.hpp"
#include "jsontool.h"


std::string generate_uuid();
std::vector<std::string> ParseMessage4(const std::string& message); 
std::vector<std::string> ParseMessageCust(const std::string& message, int num = 4, char openChar='[', char closeChar=']');

// 获取当前时间戳
uint64_t getCurrentTimestamp();

// 将时间戳转换为字符串格式
std::string timestampToString(uint64_t timestamp);

// 将字符串格式转换为时间戳
uint64_t stringToTimestamp(const std::string& datetime);

#endif
