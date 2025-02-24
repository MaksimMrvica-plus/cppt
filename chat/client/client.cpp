#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <winsock2.h>
#include "define.h"
#include "../tool/tool.h"
#include "../../chat/user/user.h"
#include "../../chat/user/userprofile.h"

#pragma comment(lib, "ws2_32.lib")

// TODO 全局变量, 本地资料准备
// 1. 加载用户资料到内存
USER user;
UserProfile user_profile;
// 2. 加载聊天记录到内存
// 3. 加载好友列表到内存
// 4. 加载群组列表到内存
void GetInputString(std::string &ss, const int MAX_LENGTH = 1024)
{
    std::string str{0};
    std::cout << "input send text:" << '\n';
    // 使用 std::getline 读取输入到字符串
    std::getline(std::cin, str);
    // 检查输入长度是否超限
    if (str.size() > MAX_LENGTH)
    {
        // 若需要强制截断
        str.resize(MAX_LENGTH);
        std::cout << "Input exceeded limit. Truncated to: " << str << std::endl;
    }
    ss = str;
}

ordered_json getMutipleUserInput(){
    ordered_json j = ordered_json::array();
    std::string type, sub_type, data;
    while ( "quit" != type)
    {
        std::cout << "Enter type for first item: ";
        std::getline(std::cin, type);
        if ("quit" == type)break;
        std::cout << "Enter sub_type for first item: ";
        std::getline(std::cin, sub_type);
        std::cout << "Enter data for first item: ";
        std::getline(std::cin, data);
        j.push_back({{"type", type}, {"sub_type", sub_type}, {"data", data}, {"extra", "备用字段"}});
    }

    return j;
}

int updateUserProfile(UserProfile &user_profile, const ordered_json &data)
{
    if (data.contains("nickname"))
    {
        user_profile.setNickname(data["nickname"]);
    }
    if (data.contains("gender"))
    {
        user_profile.setGender(data["gender"]);
    }
    if (data.contains("birthday"))
    {
        user_profile.setBirthday(data["birthday"]);
    }
    if (data.contains("bio"))
    {
        user_profile.setBio(data["bio"]);
    }
    if (data.contains("location"))
    {
        user_profile.setLocation(data["location"]);
    }
    if (data.contains("occupation"))
    {
        user_profile.setOccupation(data["occupation"]);
    }
    if (data.contains("interests"))
    {
        user_profile.setInterests(data["interests"]);
    }
    if (data.contains("education"))
    {
        user_profile.setEducation(data["education"]);
    }
    if (data.contains("website"))
    {
        user_profile.setWebsite(data["website"]);
    }
    return UPDATE_PROFILE_SUCCESS;
}

int SendReqRegisterMessage(SOCKET client_socket, const std::string &username, const std::string &password)
{
    // 创建一个 JSON 对象
    ordered_json j = createOrderedJsonMessage(CIPHER, REQ_REGISTER, username, password);
    // std::cout << j.dump(4) << std::endl;
    // 发送
    std::string send_mes = j.dump();
    int ret = send(client_socket, send_mes.c_str(), send_mes.size(), 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to send the register message procedure" << '\n';
        return FAILURE;
    }
    std::cout << "INFO|Send a message to register -->>" << '\n'
              << j.dump(4) << '\n';
    return SUCCESS;
}

int DealWithMessage(const std::string &ss)
{
    std::cout << "INFO|DealWithMessage 接收数据：\n"
              << '\n';
    ordered_json j = ordered_json::parse(ss);
    std::cout << j.dump(4) << '\n';
    std::string cipher = j["cipher"];
    std::string type = j["type"];
    std::string status = j["status"];
    if (cipher != CIPHER)
    {
        return CIPHER_ERROR;
    }
    if (type == ANS_REGISTER)
    {
        // 处理注册返回消息

        if (status == STATUS_SUCCESS)
        {
            std::cout << "INFO|接收返回消息成功，注册成功" << '\n';
            return REGISTER_SUCCESS;
        }
        else if (status == STATUS_FAILURE)
        {
            std::cout << "INFO|接收返回消息成功，注册失败 : [用户名已存在]" << '\n';
            return REGISTER_FAILURE;
        }
        else // LTODO 可以具体成数据库错误和其他错误
        {
            std::cout << "INFO|接收返回消息成功，未知错误" << '\n';
            return UNKNOWN_ERROR;
        }
        // return REGISTER_SUCCESS; // 留着测试用
    }
    else if (type == ANS_LOGIN)
    {
        if (STATUS_SUCCESS == status)
        {
            if (j["username"] == "admin") // 登录成功时，不再返回密码，只用用户名判断是否是管理员
            {
                std::cout << "INFO|接收返回消息成功，登录管理员成功" << '\n';
                return LOGIN_ADMIN_SUCCESS;
            }
            std::cout << "INFO|接收返回消息成功，登录普通账户成功" << '\n';
            return LOGIN_SUCCESS;
        }
        else
        {
            std::cout << "INFO|接收返回消息成功，登录失败" << '\n';
            return LOGIN_FAILURE;
        }
    }
    else if (ANS_USER_PROFILE == type)
    {
        if (STATUS_SUCCESS == status)
        {
            std::cout << "INFO|接收返回消息成功，获取用户资料成功" << '\n';
            // 加载到内存
            ordered_json data = j["data"];
            if (UPDATE_PROFILE_SUCCESS == updateUserProfile(user_profile, data))
            {
                std::cout << "INFO | Update User Profile [Success] !" << '\n';
            }
            else
            {
                std::cout << "ERROR | Update User Profile [Failed] !" << '\n';
            }
            return LOAD_USER_PROFILE_SUCCESS;
        }
        else if (STATUS_FAILURE == status)
        {
            std::cout << "INFO | 需要创建用户资料" << std::endl;
            return LOAD_USER_PROFILE_FAILURE;
        }
    }
    else if (ANS_CREATE_USER_PROFILE == type)
    {
        if (STATUS_SUCCESS == status)
        {
            std::cout << "INFO|接收返回消息成功，创建用户资料成功" << '\n';
            return CREATE_USER_PROFILE_SUCCESS;
        }
        else if (STATUS_FAILURE == status)
        {
            std::cout << "INFO|接收返回消息成功，创建用户资料失败, 用户资料已存在，请走更新流程" << '\n';
            return CREATE_USER_PROFILE_FAILURE;
        }
        else if (STATUS_ERROR == status)
        {
            std::cout << "INFO|接收返回消息成功，创建用户资料失败，未知错误" << '\n';
            return CREATE_USER_PROFILE_FAILURE;
        }
    }
    else
    {
        std::cout << "INFO|接收返回消息成功，未知类型" << '\n';
        return DEFAULT_ERROR;
    }

    return DEFAULT_ERROR;
}

int RecvAnsRegisterMessage(SOCKET client_socket)
{
    // 接收结果
    std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    int ret = recv(client_socket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to receive the register message procedure" << '\n';
        return FAILURE;
    }
    else
    {
        // 对返回结果进行解析，成功/失败
        int res_code = DealWithMessage(rbuffer);

        if (REGISTER_SUCCESS == res_code)
        {
            std::cout << "INFO|Register successed" << '\n';
            return REGISTER_SUCCESS;
        }
        else if (REGISTER_FAILURE == res_code)
        {
            std::cout << "ERROR|Register failed" << '\n';
            return REGISTER_FAILURE;
        }
        else
        {
            std::cout << "ERROR|Register Failed Unknown Reason" << '\n';
            return REGISTER_FAILURE;
        }
    }
}

bool CheckUserRegister(SOCKET client_socket, const std::string mes)
{
    // FLAG 暂时不用
    // 发送
    SendReqRegisterMessage(client_socket, "admin", "admin");
    std::cout << "INFO|Send a message to check whether the user already exists" << '\n';
    // 接收结果 (没有服务侧创建，返回真，否则返回假)
    std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    int ret = recv(client_socket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to receive the user check message procedure" << '\n';
        return false;
    }
    else
    { // TODO 对返回结果进行解析，成功/失败
        int res_code = DealWithMessage(rbuffer);
        if (REGISTER_SUCCESS == res_code)
        {
            std::cout << "INFO|Username Can Register" << '\n';
            return true;
        }
        else if (REGISTER_FAILURE == res_code)
        {
            std::cout << "ERROR|Username Has exitsted" << '\n';
            return false;
        }
        else
        {
            std::cout << "ERROR|Registration Failed Unknown Reason" << '\n';
            return false;
        }
    }
    return false;
}

bool CheckAccountFormat(const std::string &name, const std::string &pass)
{
    // 允许的用户名字符集合
    std::unordered_set<char> name_char_set = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '_', '.', '-'};
    // 允许的密码字符集合
    std::unordered_set<char> pass_char_set = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+'};
    bool flag = true;
    // 检查用户名长度
    if (name.size() < 8 || name.size() > 16)
    {
        std::cout << "Error|Username Length Invalid !" << std::endl;
        flag = false;
    }
    // 检查密码长度
    if (pass.size() < 8 || pass.size() > 16)
    {
        std::cout << "Error|Password Length Invalid !" << std::endl;
        flag = false;
    }
    if (!flag)
        return false; // 提前检出，防止后续麻烦
    // 检查用户名字符
    std::string errc = "";
    for (const auto &c : name)
    {
        if (name_char_set.find(c) == name_char_set.end())
            errc += c;
    }
    if (!errc.empty())
    {
        flag = false;
        std::cout << "Error|Username Char Invalid : [" << errc << ']' << std::endl;
    }

    // 检查密码字符
    errc = "";
    for (const auto &c : pass)
    {
        if (pass_char_set.find(c) == pass_char_set.end())
            errc += c;
    }
    if (!errc.empty())
    {
        std::cout << "Error|Password Char Invalid : [" << errc << ']' << std::endl;
        flag = false;
    }

    // 检查密码风险度，必须包含至少 一个大写字母或小写字母，一个数字，一个特殊符号
    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
    for (const auto &c : pass)
    {
        if (isupper(c))
            has_upper = true;
        if (islower(c))
            has_lower = true;
        if (isdigit(c))
            has_digit = true;
        if (pass_char_set.find(c) != pass_char_set.end() && !isalnum(c))
            has_special = true;
    }
    if ((!has_upper && !has_lower) || !has_digit || !has_special)
    {
        std::cout << "Error|Password must contain at least one uppercase letter, one lowercase letter, one digit, and one special character." << std::endl;
        flag = false;
    }
    return flag;
}

bool CheckLoginState(SOCKET client_socket, const std::string mes)
{
    // 发送
    send(client_socket, mes.c_str(), mes.size(), 0);
    std::cout << "INFO|Send a message to login..." << '\n';
    // 接收结果 (没有服务侧创建，返回真，否则返回假)
    std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    int ret = recv(client_socket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to receive the login message procedure" << '\n';
        return false;
    }
    // 对返回结果进行解析，成功/失败
    int res_code = DealWithMessage(rbuffer);
    if (LOGIN_SUCCESS == res_code or LOGIN_ADMIN_SUCCESS == res_code)
    {
        std::cout << "INFO|Login successed" << '\n';
        return true;
    }
    else if (LOGIN_FAILURE == res_code)
    {
        std::cout << "ERROR|Login failed" << '\n';
        return false;
    }
    else
    {
        std::cout << "ERROR|Login Failed Unknown Reason" << '\n';
        return false;
    }

    return false;
}

int CreateUserProfile(ordered_json &oj)
{
    // 询问用户依次输入以下信息 LTODO 未来可以考虑优化为循环输入，并增加一些限制
    // Nickname:
    // Gender:
    // Birthday:
    // Bio:
    // Location:
    // Occupation:
    // Interests:
    // Education:
    // Website:
    std::cout << "Enter nickname: ";
    std::string nickname;
    std::getline(std::cin, nickname);
    oj["nickname"] = nickname;

    std::cout << "Enter gender: ";
    std::string gender;
    std::getline(std::cin, gender);
    oj["gender"] = gender;

    std::cout << "Enter birthday: ";
    std::string birthday;
    std::getline(std::cin, birthday);
    oj["birthday"] = birthday;

    std::cout << "Enter bio: ";
    std::string bio;
    std::getline(std::cin, bio);
    oj["bio"] = bio;

    std::cout << "Enter location: ";
    std::string location;
    std::getline(std::cin, location);
    oj["location"] = location;

    std::cout << "Enter occupation: ";
    std::string occupation;
    std::getline(std::cin, occupation);
    oj["occupation"] = occupation;

    std::cout << "Enter interests: ";
    std::string interests;
    std::getline(std::cin, interests);
    oj["interests"] = interests;

    std::cout << "Enter education: ";
    std::string education;
    std::getline(std::cin, education);
    oj["education"] = education;

    std::cout << "Enter website: ";
    std::string website;
    std::getline(std::cin, website);
    oj["website"] = website;

    return SUCCESS;
}
bool Register(SOCKET client_socket)
{
    while (1)
    {
        std::cout << "Enter username: ";
        std::string username;
        std::getline(std::cin, username);
        // 退出
        if ("quit" == username)
            return false;

        std::cout << "Enter password: ";
        std::string password;
        std::getline(std::cin, password);

        // 格式校验
        if (!CheckAccountFormat(username, password))
        {
            std::cout << "Error|Account Format Invalid !" << std::endl;
            continue;
        }
        // 发送注册请求
        if (0 >= SendReqRegisterMessage(client_socket, username, password))
        {
            std::cout << "Error|Send Register Message Failed" << std::endl;
            continue;
        }
        // 接收注册结果
        if (0 >= RecvAnsRegisterMessage(client_socket))
        {
            std::cout << "Error|Receive Register Message Failed" << std::endl;
            continue;
        }
        else
        {
            std::cout << "Register successful!" << std::endl;
            // 注册成功，不再循环，返回上层询问
            return true;
        }
    }
    return false;
}

bool Login(SOCKET client_socket)
{
    while (1)
    {
        std::cout << "Enter username: ";
        std::string username;
        std::getline(std::cin, username);
        // 退出
        if ("quit" == username)
            return false;

        std::cout << "Enter password: ";
        std::string password;
        std::getline(std::cin, password);

        // 格式校验 LTODO 为了平时测试，先关闭，方便登录admin
        // if (!CheckAccountFormat(username, password))
        // {
        //     std::cout << "Error | Account Format Invalid !" << std::endl;
        //     continue;
        // }
        ordered_json _j = createOrderedJsonMessage(CIPHER, REQ_LOGIN, username, password);
        std::cout << "发送登录请求消息:\n"
                  << _j.dump(4) << std::endl;
        // 验证登录状态
        if (!CheckLoginState(client_socket, _j.dump()))
        {
            std::cout << "Error : Username or Password error !" << std::endl;
            continue;
        }
        else
        { // 登录成功
            std::cout << "Login successful!" << std::endl;
            // TODO 加载用户资料到内存 (应该要放到主程序里了)
            std::cout << "Load Profile ..." << std::endl;
            user.setUsername(username);
            user.setPassword(password);
            return true;
        }
    }
    return false;
}

int RegisterOrLogin(SOCKET client_socket)
{
    /*
    r or l:
        l:
            input
            return
                true
                    load profile
                false
                    again
                    -> r
        r:
            input
            return
                success
                fail
    */
    while (1)
    {
        std::cout << "Enter 'r' to register, 'l' to login, or 'quit' to exit: ";
        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "r")
        {
            Register(client_socket);
            continue; // 注册无论是否失败都返回上层询问
        }
        else if (choice == "l")
        {
            if (Login(client_socket))
                return LOGIN_SUCCESS; // 登录成功，返回 1
            else
                continue;
        }
        else if (choice == "quit")
        {
            std::cout << "Exiting program." << std::endl;
            return LOGIN_QUIT; // 取消登录，返回 -1
        }
        else
        {
            std::cout << "Invalid input. Please try again." << std::endl;
            continue;
        }
    }

    return LOGIN_FAILURE;
}

SOCKET InitializeClientSocket()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    // 1 create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "Create client socket failed, Error code: " << GetLastError() << '\n';
        return INVALID_SOCKET;
    }
    else
    {
        std::cout << "Create client socket succeeded!" << '\n';
    }

    sockaddr_in target = {0};
    target.sin_family = AF_INET;
    target.sin_port = htons(8080);                        // 大小端转换
    target.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // 字符串ip 转 整数ip

    // 2 connect
    if (-1 == connect(clientSocket, (struct sockaddr *)&target, sizeof(target)))
    {
        std::cout << "Connect server failed, Error code: " << GetLastError() << '\n';
        closesocket(clientSocket);
        return INVALID_SOCKET;
    }
    else
    {
        std::cout << "Connect server succeeded!" << '\n';
    }

    return clientSocket;
}

int main()
{

    // 1. 初始化客户端socket
    SOCKET clientSocket = InitializeClientSocket();
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "Failed to initialize client socket, exiting program..." << '\n';
        return SOCKET_ERROR;
    }

    // 2. 注册、登录验证
    int log_ret = RegisterOrLogin(clientSocket);
    if (LOGIN_QUIT == log_ret or LOGIN_FAILURE == log_ret)
    {
        std::cout << "取消 注册or登录, 即将退出程序..." << std::endl;
        Sleep(3 * 1000); // 休眠3s
        closesocket(clientSocket);
        return LOGIN_QUIT; // 用户主动取消登录，返回 11
    }
    else
    {
        std::cout << "登录成功，等待加载用户资料..." << std::endl;
    }

    // 3. 加载用户资料到本地
    // 发送资料请求消息
    ordered_json req_profile = createOrderedJsonMessage(CIPHER, "R_PROFILE", user.getUsername());
    std::cout << "INFO | 发送请求用户资料消息:\n"
              << req_profile.dump(4) << '\n';
    std::string profile_mes = req_profile.dump();
    send(clientSocket, profile_mes.c_str(), profile_mes.size(), 0);
    // 接收返回结果, 包含用户资料
    std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    int ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to receive the user profile message procedure" << '\n';
        return -1;
    }
    // 处理返回用户资料
    int res_code = DealWithMessage(rbuffer); // 如果成功，在函数内已加载到内存
    if (LOAD_USER_PROFILE_FAILURE == DealWithMessage(rbuffer))
    {
        std::cout << "是否创建用户资料?" << std::endl;
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "n" or choice == "N")
        {
            std::cout << "取消创建用户资料，跳过此步骤..." << std::endl;
        }
        else if (choice == "y" or choice == "Y")
        {
            // 创建用户资料流程
            // 1. 创建用户资料
            ordered_json ojs = {};
            int ret = CreateUserProfile(ojs);
            if (SUCCESS == ret)
            {
                // 2. 发送创建用户资料消息
                ordered_json _j = createOrderedJsonMessage(CIPHER, REQ_CREATE_USER_PROFILE, user.getUsername());
                SetOrdJsonKV(_j, std::make_pair("data", ojs));
                std::string send_mes = _j.dump();
                send(clientSocket, send_mes.c_str(), send_mes.size(), 0);
                std::cout << "INFO|Send a message to create user profile -->>" << '\n'
                          << _j.dump(4) << '\n';
                // 3. 接收返回结果
                std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
                int r_ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
                if (r_ret <= 0)
                {
                    std::cout << "ERROR|Failed to receive the create user profile message procedure" << '\n';
                    return -1;
                }
                // 处理返回用户资料, 加载到内存
                if (CREATE_USER_PROFILE_SUCCESS == DealWithMessage(rbuffer))
                {
                    std::cout << "INFO|Create User Profile Success !" << '\n';
                    std::cout << "INFO|Loading User Profile ..." << '\n';
                    ordered_json _data = ordered_json::parse(rbuffer)["data"];
                    updateUserProfile(user_profile, _data);
                }
                else
                {
                    std::cout << "ERROR|创建用户资料失败，跳过此步骤... !" << '\n';
                }
            }
        }
        else{
            std::cout << "输入错误，跳过此步骤... !" << std::endl;
        }
    }
    std::cout << "当前用户最新资料" << std::endl;
    user_profile.displayUserProfile();

    // TODO*** 4. 聊天功能
    ordered_json input_content = getMutipleUserInput();
    std::cout << input_content.dump(4) << std::endl;
    //
    // 组装data部分
    ordered_json data = createSendMessageJson(generate_uuid(), user.getIntUserID(), 0);
    SetOrdJsonKV(data, std::make_pair("content", input_content));
    ordered_json sys_mes = createOrderedJsonMessage(CIPHER, REQ_SEND_MESSAGE, user.getUsername(), "", data);
    std::string send_mes = sys_mes.dump();
    send(clientSocket, send_mes.c_str(), send_mes.size(), 0);
    std::cout << "INFO|发送聊天消息 -->>" << '\n'
              << sys_mes.dump(4) << '\n';
    
    // 3 send
    while (1)
    {
        std::string sbuffer = {0};
        GetInputString(sbuffer, MAX_MESSAGE_LENGTH);
        send(clientSocket, sbuffer.c_str(), sbuffer.size(), 0);
        std::cout << "send success success !" << '\n';

        std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
        int ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
        if (ret <= 0)
            break;
        std::cout << "receive message:" << '\n'
                  << rbuffer << std::endl;
    }

    // 4 close
    closesocket(clientSocket);

    return 0;
}

/*

g++ client.cpp -o client.exe -L ../server  -l sqlite3
g++ -o client client.cpp -lws2_32
g++ client.cpp -o client.exe -I include -L . -l sqlite3 -lws2_32
要带着其他自定义库文件一起编译

g++ -o client client.cpp ../tool/tool.cpp ../tool/jsontool.cpp ../../chat/user/user.cpp ../../chat/user/userprofile.cpp -lws2_32 -lrpcrt4

*/