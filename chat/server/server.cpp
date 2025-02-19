#include <iostream>
#include <string>
#include <winsock2.h>
#include <vector>
#include "sqlite3.h"
#include "../tool/tool.h"
#include "../client/define.h"
#include "../../chat/user/user.h"
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

int QueryLoginInUser(const std::string& username, const std::string& password) {
    sqlite3* db;
    const char* dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return OPEN_DB_FAILURE;
    }

    std::string sql = "SELECT password FROM users WHERE username = ?";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_USER_NOT_EXIST;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* dbPassword = sqlite3_column_text(stmt, 0);
        if (password == reinterpret_cast<const char*>(dbPassword)) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            std::cout << "Login success" << std::endl;
            return LOGIN_SUCCESS;
        } else {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            std::cout << "Password error" << std::endl;
            return SQL_USER_PASSWORD_ERROR;
        }
    } else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "User not exist" << std::endl;
        return SQL_USER_NOT_EXIST;
    }
}

int QueryUserProfile(const std::string& username, ordered_json& js) {
    sqlite3* db;
    const char* dbPath = R"(..\..\db\user.db)";
    
    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }

    std::string sql = "SELECT * FROM user_profiles WHERE username = ?;";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt);  // 获取列数
        for (int i = 0; i < column_count; ++i) {        // 遍历每一列
            const char* column_name = sqlite3_column_name(stmt, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            js[column_name] = column_value ? column_value : "";
        }

        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_EXIST; // 用户资料存在
    } else {
        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_NOT_EXIST; // 用户资料不存在
    }
}

int DealWithMessage(const std::string &ss, SOCKET clientSocket)
{
    std::cout << "INFO|DealWithMessage" << '\n';
    ordered_json j = ordered_json::parse(ss);
    std::cout << j.dump(4) << '\n';
    std::string cipher  = j["cipher"];
    std::string type    = j["type"];
    if (cipher != CIPHER)return CIPHER_ERROR;
    
    std::string username = j["username"];
    std::string password = j["password"];
    if (REQ_REGISTER == type) // 处理注册请求
    {
        // TODO 查询数据库，判断用户是否存在
        // 1. 存在，返回注册失败   status = STATUS_FAILURE
        // 2. 不存在，插入数据，返回注册成功  status = STATUS_SUCCESS

    }
    else if(REQ_LOGIN == type) // 处理登录请求
    {
        ordered_json j = createOrderedJsonMessage();    // 预组装返回消息
        SetOrdJsonKV(j, std::make_pair("cipher", CIPHER));
        SetOrdJsonKV(j, std::make_pair("type", ANS_LOGIN));
        SetOrdJsonKV(j, std::make_pair("username", username));
        // 查询数据库
        int _ret = QueryLoginInUser(username, password);
        if (LOGIN_SUCCESS != _ret){  // 登录失败
            std::cout << "INFO| Load User Account From DB [Failed] [" << username << "]" << std::endl;
            SetOrdJsonKV(j, std::make_pair("message", "Login failed"));
            SetOrdJsonKV(j, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = j.dump();
            std::cout << "INFO| return user profile 发送登录应答消息:" << "\n" << j.dump(4) << std::endl; 
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return LOGIN_FAILURE;
        }
        else                         // 登录成功
        {   
            // TODO 更新登录时间 (预计放到客户端去进行请求，或者在服务端登录校对过程中进行更新（也就是这里）)

            // 返回登录成功消息
            std::cout << "INFO| Load User Account From DB [Success] [" << username << "]" << std::endl;

            SetOrdJsonKV(j, std::make_pair("message", "Login success"));
            SetOrdJsonKV(j, std::make_pair("status", STATUS_SUCCESS));
            std::string res_mes = j.dump();
            std::cout << "INFO| return user profile 发送登录应答消息:" << "\n" << j.dump(4) << std::endl; 
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return LOGIN_SUCCESS;
        }

    }
    else if(REQ_USER_PROFILE == type) // 处理请求用户资料  TODO
    {

        // TODO 查询user_profiles表
        ordered_json _js;
        int _ret_qup = QueryUserProfile(username, _js);
        ordered_json _ojs = createOrderedJsonMessage();
        SetOrdJsonKV(_ojs, std::make_pair("cipher", CIPHER));
        SetOrdJsonKV(_ojs, std::make_pair("type", ANS_USER_PROFILE));
        SetOrdJsonKV(_ojs, std::make_pair("username", username));
        SetOrdJsonKV(_ojs, std::make_pair("password", password));
        SetOrdJsonKV(_ojs, std::make_pair("data", _js));
        if (SQL_USER_PROFILE_EXIST == _ret_qup)
        {
            std::cout << "INFO| Load Profile From DB Success [" << username << "]" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_SUCCESS));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Return User Profile" << "\n" << _ojs.dump(4) << std::endl;
        }
        else
        {
            // TODO 发送消息，用户资料不存在, 提示创建用户资料
            std::cout << "INFO| Load Profile From DB Failed [" << username << "], Please Create Profile ..." << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Remind User to Create Profile" << "\n" << _ojs.dump(4) << std::endl;
        }
    }
    else if(REQ_LOGOUT == type) // 处理登出请求
    {

    }
    else if(REQ_CHAT == type) // 处理聊天请求
    {

    }
    else if(REQ_GROUP == type) // 处理群组请求
    {

    }
    else if(REQ_FRIEND == type) // 处理好友请求
    {

    }
    else
    {
        return DEFAULT_ERROR;
    }

    return DEFAULT_ERROR;
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
        std::cout << "接收消息来自接口ID :" << clientSocket << std::endl;
        DealWithMessage(buffer, clientSocket);
        // TODO 处理各种消息逻辑


        // 复读机
        //std::string str = std::string(buffer);
        //send(clientSocket, str.c_str(), str.size(), 0);
    }
    std::cout << clientSocket << "退出服务器" << std::endl;
    closesocket(clientSocket);

    return 0;
}

int main()
{
    // 1. 打开数据库
    sqlite3 *user_db = nullptr;
    int ret_db = OpenDB(user_db, R"(..\..\db\user.db)");    // 账户表
    if (OPEN_DB_SUCCESS != ret_db)return -1; // 读取失败
    sqlite3 *profile_db = nullptr;
    ret_db = OpenDB(profile_db, R"(..\..\db\profile.db)");  // 用户资料表
    if (OPEN_DB_SUCCESS != ret_db)return -1; // 读取失败

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

// #include "../tool/tool.h"
// #include "../client/define.h"
// #include "../../chat/user/user.h"
// g++ -o server server.cpp -lws2_32
// g++ -o server server.cpp -lws2_32 -lsqlite3

// g++ server.cpp  ../tool/tool.cpp ../tool/jsontool.cpp ../../chat/user/user.cpp  -o server.exe -I include -L . -l sqlite3 -lws2_32 # 之后编译只要这步即可

// chcp 65001