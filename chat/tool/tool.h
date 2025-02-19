#ifndef _TOOL_H
#define _TOOL_H
#include <vector>
#include <string>
#include "json.hpp"
#include "jsontool.h"

std::vector<std::string> ParseMessage4(const std::string& message); 
std::vector<std::string> ParseMessageCust(const std::string& message, int num = 4, char openChar='[', char closeChar=']');





#endif
