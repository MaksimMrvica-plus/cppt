#include "../user/user.h"  // 包含你的 USER 类定义
#include <iostream>

int main() {
    // 创建一个 USER 对象
    USER user("1", "testuser", "password123", "test@example.com", "2024-10-01 12:00:00");

    // 显示初始的最后登录时间
    std::cout << "Initial Last Login Time: " << user.getLastLoginTime() << std::endl;

    // 更新最后登录时间
    user.updateLastLoginTime();

    // 显示更新后的最后登录时间
    std::cout << "Updated Last Login Time: " << user.getLastLoginTime() << std::endl;

    return 0;
}