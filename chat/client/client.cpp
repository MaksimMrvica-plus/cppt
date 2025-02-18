#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <winsock2.h>
#include "define.h"
#include "../tool/tool.h"
#include "../../chat/user/user.h"
#pragma comment(lib, "ws2_32.lib")

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

int DealWithMessage(const std::string &ss)
{
    std::vector<std::string> res = ParseMessageCust(ss, 4, '[', ']');
    std::string cipher = res[0];
    std::string type = res[1];
    std::string username = res[2];
    std::string password = res[3];
    if (cipher != CIPHER_STR)
    {
        return CIPHER_ERROR;
    }
    std::cout << type << '\t' << username << '\t' << password << std::endl;
    if (type == "REGISTER" && username == "admin" && password == "admin")
    {
        std::cout << "INFO|接收返回消息成功，注册管理员成功" << '\n';
        return REGISTER_SUCCESS;
    }
    else if (type == "REGISTER")
    {
        std::cout << "INFO|注册失败" << '\n';
        return REGISTER_FAILURE;
    }
    else if (type == "LOGIN" && username == "admin" && password == "admin")
    {
        std::cout << "INFO|接收返回消息成功，登录管理员成功" << '\n';
        return LOGIN_SUCCESS;
    }
    else if (type == "LOGIN")
    {
        std::cout << "INFO|登录失败" << '\n';
        return LOGIN_FAILURE;
    }

    return DEFAULT_ERROR;
}

bool CheckUserRegister(SOCKET client_socket, const std::string mes)
{
    // 发送
    send(client_socket, mes.c_str(), mes.size(), 0);
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
    if (LOGIN_SUCCESS == res_code)
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
        std::string reg_mes = CIPHER + "[REGISTER][" + username + "][" + password + "]";

        // 执行注册等待服务侧返回结果
        if (!CheckUserRegister(client_socket, reg_mes))
        {
            std::cout << "Error|User registration failed" << std::endl;
            continue;
        }
        else
        { // TODO* 不存在,注册成功,3s后返回
            //
            std::cout << "INFO|User registration succeeded" << std::endl;
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

        // 格式校验
        if (!CheckAccountFormat(username, password))
        {
            std::cout << "Error | Account Format Invalid !" << std::endl;
            continue;
        }
        std::string log_mes = CIPHER + "[LOGIN][" + username + "][" + password + "]";

        // 验证登录状态
        if (!CheckLoginState(client_socket, log_mes))
        {
            std::cout << "Error : Username or Password error !" << std::endl;
            continue;
        }
        else
        { // 登录成功
            std::cout << "Login successful!" << std::endl;
            // TODO 加载用户资料到内存
            std::cout << "Load Profile ..." << std::endl;
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

    return 0;
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
    // TODO 本地资料准备
    // 1. 加载用户资料到内存
    USER user;
    // 2. 加载聊天记录到内存
    // 3. 加载好友列表到内存
    // 4. 加载群组列表到内存

    // 1. 初始化客户端socket
    SOCKET clientSocket = InitializeClientSocket();
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "Failed to initialize client socket, exiting program..." << '\n';
        return SOCKET_ERROR;
    }

    // 2. 注册、登录验证
    int log_ret = RegisterOrLogin(clientSocket);
    if (LOGIN_QUIT == log_ret)
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

    // 3. TODO 加载用户资料到本地
    // 发送资料请求消息
    std::string profile_mes = CIPHER + "[R_PROFILE]" + "[" + user.getUsername() + "][]";
    send(clientSocket, profile_mes.c_str(), profile_mes.size(), 0);

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

    closesocket(clientSocket);
    // 4 close

    return 0;
}

// g++ -o client client.cpp -lws2_32