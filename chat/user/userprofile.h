#ifndef _USER_PROFILE_H_
#define _USER_PROFILE_H_

#include <string>

class UserProfile {
public:
    // 构造函数
    UserProfile(const std::string& nickname = "",
                const std::string& gender = "",
                const std::string& birthday = "",
                const std::string& bio = "",
                const std::string& location = "",
                const std::string& occupation = "",
                const std::string& interests = "",
                const std::string& education = "",
                const std::string& website = "");

    // 访问器函数
    std::string getNickname() const;
    std::string getGender() const;
    std::string getBirthday() const;
    std::string getBio() const;
    std::string getLocation() const;
    std::string getOccupation() const;
    std::string getInterests() const;
    std::string getEducation() const;
    std::string getWebsite() const;

    // 设置器函数
    void setNickname(const std::string& nickname);
    void setGender(const std::string& gender);
    void setBirthday(const std::string& birthday);
    void setBio(const std::string& bio);
    void setLocation(const std::string& location);
    void setOccupation(const std::string& occupation);
    void setInterests(const std::string& interests);
    void setEducation(const std::string& education);
    void setWebsite(const std::string& website);

private:
    std::string nickname;     // 昵称
    std::string gender;       // 性别
    std::string birthday;     // 生日
    std::string bio;          // 个人简介
    std::string location;     // 地区
    std::string occupation;   // 职业
    std::string interests;    // 兴趣爱好
    std::string education;    // 教育背景
    std::string website;      // 个人网站
};

#endif