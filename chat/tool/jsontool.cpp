#include "jsontool.h"

using json = nlohmann::json;

// 函数：修改或添加键值对
void ModifyOrAdd(json& j, const std::pair<std::string, json>& kv, int overwrite) {
    const std::string& key = kv.first;
    const json& value = kv.second;

    // 检查键是否存在
    if (j.contains(key)) {
        if (overwrite == 1) {
            // 键存在且允许覆盖，修改值
            j[key] = value;
            std::cout << "Modified existing key: " << key << std::endl;
        } else {
            // 键存在但不允许覆盖，跳过
            std::cout << "Key already exists and overwrite is disabled. Skipping: " << key << std::endl;
        }
    } else {
        // 键不存在，添加键值对
        j[key] = value;
        std::cout << "Added new key: " << key << std::endl;
    }
}

int main() {
    // 创建一个示例 JSON 对象
    json j = {
        {"name", "John"},
        {"age", 30},
        {"city", "New York"}
    };

    // 打印原始 JSON
    std::cout << "Original JSON:" << std::endl;
    std::cout << j.dump(4) << std::endl;

    // 修改或添加键值对
    ModifyOrAdd(j, {"age", 31}, 1);  // 修改现有键（允许覆盖）
    ModifyOrAdd(j, {"country", "USA"}, 1);  // 添加新键
    ModifyOrAdd(j, {"city", "Los Angeles"}, 0);  // 修改现有键（不允许覆盖）

    // 打印修改后的 JSON
    std::cout << "\nModified JSON:" << std::endl;
    std::cout << j.dump(4) << std::endl;

    return 0;
}