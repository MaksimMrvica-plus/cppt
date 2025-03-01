#include "user.h"
#include <ctime>
#include <iomanip>
#include <stdio.h>
// 构造函数的实现
USER::USER(const std::string& user_id,
           const std::string& username,
           const std::string& password, 
           const std::string& email,
           const std::string& register_time, 
           const std::string& last_login_time)
        : user_id(user_id), username(username), password(password), email(email), 
          register_time(register_time), last_login_time(last_login_time) {}
// displayUserInfo 的实现
void USER::displayUserInfo() const {
    std::cout << "User ID: " << getUserID() << std::endl;
    std::cout << "Username: " << getUsername() << std::endl;
    std::cout << "Password: " << "****************" << std::endl; // 隐藏密码
    std::cout << "Email: " << getEmail() << std::endl;
    std::cout << "Register Time: " << getRegisterTime() << std::endl;
    std::cout << "Last Login Time: " << getLastLoginTime() << std::endl;
}

// updateLastLoginTime 的实现
void USER::updateLastLoginTime() {
    // 注意：std::time_t 是一个时间类型，而不是 std::string
    // 如果你需要将其转换为字符串，可以使用 std::put_time 或其他方式
    char buffer[20];
    std::time_t now = std::time(nullptr);
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    setLastLoginTime(std::string(buffer));
}