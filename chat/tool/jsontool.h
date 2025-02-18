#ifndef _JSONTOOL_H
#define _JSONTOOL_H

#include <iostream>
#include <utility>  // std::pair
#include "json.hpp"

using json = nlohmann::json;

void ModifyOrAdd(json& j, const std::pair<std::string, json>& kv, int overwrite=1); // 默认覆盖

#endif