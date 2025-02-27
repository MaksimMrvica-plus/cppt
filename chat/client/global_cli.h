#ifndef GLOBALS_H
#define GLOBALS_H

#include <winsock2.h>
#include "../user/user.h"
#include "../user/userprofile.h"
#include "../tool/messagequeue.h"
#include <unordered_map>

extern SOCKET clientSocket;
extern USER user;
extern UserProfile user_profile;
extern MessageQueue<std::string> sendQueue;
extern MessageQueue<std::string> recvQueue;
extern std::unordered_map<std::string, u_int64> user2id;
extern std::unordered_map<std::string, u_int64> TEST_USED_user2id;
extern std::unordered_set<std::string> TEST_USED_friend_username_uord_set;

#endif // GLOBALS_H