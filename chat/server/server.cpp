#include <iostream>
#include <string>
#include <winsock2.h>
#include "sqlite3.h"
#include "D:\cppprojects\cppt\chat\client\define.h"
#pragma comment(lib, "ws2_32.lib")

SOCKET InitializeServerSocket()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup failed, Error code: " << GetLastError() << '\n';
        return INVALID_SOCKET;
    }

    // 1. 创建套接字
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listenSocket)
    {
        std::cout << "Create listen socket failed, Error code: " << GetLastError() << '\n';
        WSACleanup();
        return INVALID_SOCKET;
    }
    else
    {
        std::cout << "Create listen socket succeeded!" << '\n';
    }

    // 2. 绑定套接字
    sockaddr_in local = {0};
    local.sin_family = AF_INET;
    local.sin_port = htons(8080);                   // 大小端转换
    local.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 接收地址

    if (bind(listenSocket, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
    {
        std::cout << "Bind listen socket failed, Error code: " << GetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    else
    {
        std::cout << "Bind listen socket succeeded!" << '\n';
    }

    // 3. 开始监听
    if (listen(listenSocket, 10) == SOCKET_ERROR)
    {
        std::cout << "Start listen failed, Error code: " << GetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    else
    {
        std::cout << "Start listen socket succeeded!" << '\n';
    }

    return listenSocket;
}

int OpenDB(sqlite3 *&db, std::string db_path)
{
    // 打开数据库
    int rc = sqlite3_open(db_path.c_str(), &db); // 使用正斜杠作为路径分隔符
    if (rc != SQLITE_OK)
    {
        if (db)
        { // 确保 db 不是 nullptr
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db); // 关闭数据库
        }
        else
        {
            std::cerr << "Cannot open database: sqlite3_open failed to initialize db pointer." << std::endl;
        }
        return OPEN_DB_FAILURE;
    }
    else
    {
        std::cout << "Open database successfully :\n -> \t [ " << db_path << " ]" << std::endl;
    }
    return OPEN_DB_SUCCESS;
}

DWORD WINAPI ThreadFunc(LPVOID lpThreadParameter)
{
    SOCKET clientSocket = *(SOCKET *)lpThreadParameter;
    free(lpThreadParameter);

    std::cout << clientSocket << "连入服务器" << std::endl;
    while (1)
    {
        char buffer[1024] = {0};

        int ret = recv(clientSocket, buffer, 1024, 0);
        if (ret <= 0)
            break;
        std::cout << "接口ID :" << clientSocket << '\t' << "内容 : " << buffer << std::endl;
        // TODO DealMessage
        // 处理各种消息逻辑
        std::string str = std::string(buffer);
        send(clientSocket, str.c_str(), str.size(), 0);
    }
    std::cout << clientSocket << "退出服务器" << std::endl;
    closesocket(clientSocket);

    return 0;
}

int main()
{
    // 1. 打开数据库
    sqlite3 *db = nullptr;
    std::string db_path = R"(D:\cppprojects\cppt\db\user.db)";
    int ret_db = OpenDB(db, db_path);
    if (OPEN_DB_SUCCESS != ret_db)
    {
        return -1; // 读取失败
    }

    // 2. 初始化服务器socket
    SOCKET listenSocket = InitializeServerSocket();
    if (INVALID_SOCKET == listenSocket)
    {
        std::cout << "Failed to initialize server socket, exiting program..." << '\n';
        return SOCKET_ERROR;
    }

    // 3. 服务器监听
    while (1)
    {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (INVALID_SOCKET == clientSocket)
            continue;

        SOCKET *sockfd = new SOCKET;
        *sockfd = clientSocket;

        CreateThread(NULL, 0, ThreadFunc, sockfd, 0, NULL);
    }

    return 0;
}

// g++ -o server server.cpp -lws2_32
// g++ -o server server.cpp -lws2_32 -lsqlite3

// g++ server.cpp -o server.exe -I include -L . -l sqlite3 -lws2_32 # 之后编译只要这步即可

// chcp 65001