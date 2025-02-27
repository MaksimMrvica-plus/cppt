#ifndef CLITOOL_H
#define CLITOOL_H


#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <winsock2.h>
#include "../client/define.h"
#include "../client/global_cli.h"
#include "../tool/tool.h"
#include "../tool/messagequeue.h"
#include "../../chat/user/user.h"
#include "../../chat/user/userprofile.h"

#pragma comment(lib, "ws2_32.lib")


// 函数声明
int DealWithMessage(const std::string &ss, SOCKET client_socket=0);
int GetProfile(const std::string &username);
int UpdateProfile(const std::string &username, const ordered_json &data);
int CreateProfile(const std::string &username, const ordered_json &data);
int SendReqRegisterMessage(SOCKET client_socket, const std::string &username, const std::string &password);
int ProcessSuccess_ANS_LOGIN(SOCKET client_socket, const ordered_json &j);
int updateUserProfile(UserProfile &user_profile, const ordered_json &data);
int ChooseOperation();
void GetInputString(std::string &ss, const int MAX_LENGTH);
ordered_json getMutipleUserInputJson();
bool CheckUserRegister(SOCKET client_socket, const std::string mes);
bool CheckAccountFormat(const std::string &name, const std::string &pass);
bool CheckLoginState(SOCKET client_socket, const std::string mes);
int CreateUserProfile(ordered_json &oj);
int SendMessagePer();
std::string ChooseSendFriend();
bool Register(SOCKET client_socket);
bool Login(SOCKET client_socket);
bool Logout(SOCKET client_socket);
bool Chat(SOCKET client_socket);
bool Group(SOCKET client_socket);
bool Friend(SOCKET client_socket);
bool SendMsg(SOCKET client_socket);
void SendThread(SOCKET client_socket);
void ReceiveThread(SOCKET client_socket);
void ProcessThread(SOCKET client_socket);
int DealWithOperation(int opt, SOCKET client_socket);
SOCKET InitializeClientSocket();
std::unordered_set<std::string> GetFriendUsernameUordSet();
void ShowFriendUsernameList(std::unordered_set<std::string>& us);  // 后续重载函数，接收多种类型




#endif
