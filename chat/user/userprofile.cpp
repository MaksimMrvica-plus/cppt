#include "userprofile.h"

// 构造函数
UserProfile::UserProfile(const std::string& nickname,
                         const std::string& gender,
                         const std::string& birthday,
                         const std::string& bio,
                         const std::string& location,
                         const std::string& occupation,
                         const std::string& interests,
                         const std::string& education,
                         const std::string& website)
    : nickname(nickname), gender(gender), birthday(birthday), bio(bio), location(location),
      occupation(occupation), interests(interests), education(education), website(website) {}

// 访问器函数

std::string UserProfile::getNickname() const {
    return nickname;
}

std::string UserProfile::getGender() const {
    return gender;
}

std::string UserProfile::getBirthday() const {
    return birthday;
}

std::string UserProfile::getBio() const {
    return bio;
}

std::string UserProfile::getLocation() const {
    return location;
}

std::string UserProfile::getOccupation() const {
    return occupation;
}

std::string UserProfile::getInterests() const {
    return interests;
}

std::string UserProfile::getEducation() const {
    return education;
}

std::string UserProfile::getWebsite() const {
    return website;
}

// 设置器函数
void UserProfile::setNickname(const std::string& nickname) {
    this->nickname = nickname;
}
void UserProfile::setGender(const std::string& gender) {
    this->gender = gender;
}

void UserProfile::setBirthday(const std::string& birthday) {
    this->birthday = birthday;
}

void UserProfile::setBio(const std::string& bio) {
    this->bio = bio;
}

void UserProfile::setLocation(const std::string& location) {
    this->location = location;
}

void UserProfile::setOccupation(const std::string& occupation) {
    this->occupation = occupation;
}

void UserProfile::setInterests(const std::string& interests) {
    this->interests = interests;
}

void UserProfile::setEducation(const std::string& education) {
    this->education = education;
}

void UserProfile::setWebsite(const std::string& website) {
    this->website = website;
}