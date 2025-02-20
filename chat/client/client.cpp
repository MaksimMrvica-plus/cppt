#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <winsock2.h>
#include "define.h"
#include "../tool/tool.h"
#include "../../chat/user/user.h"
#pragma comment(lib, "ws2_32.lib")


// TODO 全局变量, 本地资料准备
// 1. 加载用户资料到内存
USER user;
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

int SendReqRegisterMessage(SOCKET client_socket, const std::string &username, const std::string &password)
{
    // 创建一个 JSON 对象
    ordered_json j = createOrderedJsonMessage(CIPHER, REQ_REGISTER, username, password);
    // std::cout << j.dump(4) << std::endl;
    // 发送
    std::string send_mes = j.dump();
    send(client_socket, send_mes.c_str(), send_mes.size(), 0);
    std::cout << "INFO|Send a message to register -->>" << '\n' << j.dump(4) << '\n';
    return SUCCESS;
}



int DealWithMessage(const std::string &ss)
{
    std::cout << "INFO|DealWithMessage 接收数据：\n" << '\n';
    ordered_json j = ordered_json::parse(ss);
    std::cout << j.dump(4) << '\n';
    std::string cipher  = j["cipher"];
    std::string type    = j["type"];
    if (cipher != CIPHER)
    {
        return CIPHER_ERROR;
    }
    if (type == ANS_REGISTER)
    {
        // TODO 处理注册返回消息
        std::string status = j["status"];
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
        else
        {
            std::cout << "INFO|接收返回消息成功，未知状态" << '\n';
            return DEFAULT_ERROR;
        }
        return REGISTER_SUCCESS; // 留着测试用
    }
    else if (type == ANS_LOGIN)
    {
        if(STATUS_SUCCESS == j["status"]){ 
            if (j["username"] == "admin")   // 登录成功时，不再返回密码，只用用户名判断是否是管理员
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
    else if (type == ANS_USER_PROFILE)
    {
        // TODO 用户资料逻辑
        std::cout << "INFO|接收返回消息成功，用户资料" << '\n';
        return SUCCESS;
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
        // TODO 对返回结果进行解析，成功/失败
        int res_code = DealWithMessage(rbuffer);


        if (REGISTER_SUCCESS == res_code)
        {
            std::cout << "INFO|Register successed" << '\n';
            return SUCCESS;
        }
        else if (REGISTER_FAILURE == res_code)
        {
            std::cout << "ERROR|Register failed" << '\n';
            return FAILURE;
        }
        else
        {
            std::cout << "ERROR|Register Failed Unknown Reason" << '\n';
            return FAILURE;
        }
    }
    return FAILURE;
}

bool CheckUserRegister(SOCKET client_socket, const std::string mes)
{
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

bool CheckAccountFormat(const std::string name, const std::string pass)
{
    // TODO 检查账户基本格式正确
    // 在这里面输出错误信息
    return true;
}

bool CheckLoginState(SOCKET client_socket, const std::string mes)
{
    // TODO
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
    // TODO 对返回结果进行解析，成功/失败
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
        if (!SendReqRegisterMessage(client_socket, username, password))
        {
            std::cout << "Error|Send Register Message Failed" << std::endl;
            continue;
        }
        // 接收注册结果
        if (!RecvAnsRegisterMessage(client_socket))
        {
            std::cout << "Error|Receive Register Message Failed" << std::endl;
            continue;
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

        // 格式校验
        if (!CheckAccountFormat(username, password))
        {
            std::cout << "Error | Account Format Invalid !" << std::endl;
            continue;
        }
        ordered_json _j = createOrderedJsonMessage(CIPHER, REQ_LOGIN, username, password);
        std::cout << "发送登录请求消息:\n" << _j.dump(4) << std::endl;
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
    std::cout << "INFO | 发送请求用户资料消息:\n" << req_profile.dump(4) << '\n';
    std::string profile_mes = req_profile.dump();
    send(clientSocket, profile_mes.c_str(), profile_mes.size(), 0);
    // 接收返回结果, 包含用户资料, 加载到内存
    std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    int ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    if (ret <= 0)
    {
        std::cout << "ERROR|Failed to receive the user profile message procedure" << '\n';
        return -1;
    }
    else
    {
        std::cout << "INFO|Receive user profile message success !" << '\n';
        std::cout << ordered_json::parse(rbuffer).dump(4) << '\n';
    }
    // TODO*** 加载到内存

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

// g++ client.cpp -o client.exe -L ../server  -l sqlite3
// g++ -o client client.cpp -lws2_32
// g++ client.cpp -o client.exe -I include -L . -l sqlite3 -lws2_32
// 要带着其他自定义库文件一起编译
// g++ -o client client.cpp ../tool/tool.cpp ../tool/jsontool.cpp ../../chat/user/user.cpp -lws2_32