#ifndef _USER_H_
#define _USER_H_

#include <string>
#include <ctime>
#include <cstdint>
#include <iostream>

class USER {
private:
    // 成员变量
    std::string user_id;         // 用户ID，自增主键
    std::string username;        // 用户名，唯一
    std::string password;        // 密码（加密存储）
    std::string email;           // 邮箱（可选）
    std::string register_time;   // 注册时间，默认为当前时间
    std::string last_login_time; // 最后登录时间
public:
    // 构造函数
    USER(const std::string& user_id = "0",
        const std::string& username = "",
        const std::string& password = "", 
        const std::string& email = "",
        const std::string& register_time = "", 
        const std::string& last_login_time = "");

    // 成员函数
    void displayUserInfo() const;
    void updateLastLoginTime();
    // 访问器
    std::string getUserID() const { return user_id; }
    void setUserID(const std::string& id) { user_id = id; }

    // 返回整型格式用户ID
    int64_t getIntUserID() const { return std::stoll(user_id); }

    std::string getUsername() const { return username; }
    void setUsername(const std::string& name) { username = name; }

    std::string getPassword() const { return password; }
    void setPassword(const std::string& pwd) { password = pwd; }

    std::string getEmail() const { return email; }
    void setEmail(const std::string& mail) { email = mail; }

    std::string getRegisterTime() const { return register_time; }
    void setRegisterTime(const std::string& time) { register_time = time; }

    std::string getLastLoginTime() const { return last_login_time; }
    void setLastLoginTime(const std::string& time) { last_login_time = time; }
};

#endif