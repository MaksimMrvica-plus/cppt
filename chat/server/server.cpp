#include <iostream>
#include <string>
#include <unordered_map>
#include <winsock2.h>
#include <vector>
#include <thread>
#include <mutex>
#include "sqlite3.h"
#include "../tool/tool.h"
#include "../client/define.h"
#include "../../chat/user/user.h"
#pragma comment(lib, "ws2_32.lib")

std::unordered_map<std::string, SOCKET> username2socket_map; // 当前在线 用户名映射端口ID
std::mutex mapMutex;
std::unordered_map<int, std::string> userid2username_map; // user ID 映射用户名
std::unordered_map<std::string, int> username2userid_map; // 用户名映射user ID

void AddUnameAndSocketMap(const std::string &username, SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    username2socket_map[username] = clientSocket;
}

void RemoveUnameAndSocketMap(const std::string &username)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    username2socket_map.erase(username);
}

SOCKET GetUserSocket(const std::string &username)
{
    std::lock_guard<std::mutex> lock(mapMutex);
    if (username2socket_map.find(username) != username2socket_map.end())
    {
        return username2socket_map[username];
    }
    return INVALID_SOCKET;
}

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
    local.sin_port = htons(SERVER_PORT);            // 大小端转换
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

int RegisterUser(const std::string &username, const std::string &password)
{
    /**
     * @brief 注册用户
     *
     * 该函数用于在数据库中注册新用户。首先，它会检查用户是否已经存在于数据库中。
     * 如果用户已存在，函数将返回 `SQL_USER_EXIST`。如果用户不存在，函数将插入新用户并返回 `REGISTER_SUCCESS`。
     *
     * @param username 用户名
     * @param password 密码
     * @return int 返回码，表示操作结果
     *         - `REGISTER_SUCCESS`：注册成功
     *         - `SQL_USER_EXIST`：用户已存在
     *         - `OPEN_DB_FAILURE`：无法打开数据库
     *         - `SQL_ERROR`：SQL 语句执行错误
     */
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return OPEN_DB_FAILURE;
    }

    // 查询用户是否存在
    std::string sql = "SELECT username FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_ROW)
    {
        // 用户已存在
        sqlite3_close(db);
        std::cout << "INFO | 注册失败，用户已存在" << std::endl;
        return SQL_USER_EXIST;
    }
    else
    {
        // 用户不存在，插入新用户
        sql = "INSERT INTO users (username, password) VALUES (?, ?)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            std::cerr << "INFO | Failed to prepare insert statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return SQL_ERROR;
        }

        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "INFO | Failed to insert new user: " << sqlite3_errmsg(db) << std::endl;
            return SQL_ERROR;
        }
        std::cout << "INFO | 成功插入输入到用户表" << std::endl;
        std::cout << "INFO | 注册成功" << std::endl;
        return REGISTER_SUCCESS;
    }
}

int QueryLoginInUser(const std::string &username, const std::string &password)
{
    // >=0,查到用户，返回ID
    // <0, 校验失败，返回其他错误码
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return OPEN_DB_FAILURE;
    }

    std::string sql = "SELECT user_id, password FROM users WHERE username = ?";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_USER_NOT_EXIST;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        const unsigned char *dbPassword = sqlite3_column_text(stmt, 1);
        if (password == reinterpret_cast<const char *>(dbPassword))
        {
            std::cout << "Login success" << std::endl;
            int user_id = sqlite3_column_int(stmt, 0);
            std::cout << "INFO | 登录成功，用户ID为：" << user_id << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return user_id;
        }
        else
        {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            std::cout << "Password error" << std::endl;
            return SQL_USER_PASSWORD_ERROR;
        }
    }
    else
    {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        std::cout << "User not exist" << std::endl;
        return SQL_USER_NOT_EXIST;
    }
}

int CheckUserProfileExists(const std::string &username)
{
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }

    std::string sql = "SELECT 1 FROM user_profiles WHERE username = ?;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_EXIST; // 用户资料存在
    }
    else
    {
        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_NOT_EXIST; // 用户资料不存在
    }
}

int GetUserProfile(const std::string &username, ordered_json &js)
{
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }

    std::string sql = "SELECT * FROM user_profiles WHERE username = ?;";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        int column_count = sqlite3_column_count(stmt); // 获取列数
        for (int i = 0; i < column_count; ++i)
        { // 遍历每一列
            const char *column_name = sqlite3_column_name(stmt, i);
            const char *column_value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            js[column_name] = column_value ? column_value : "";
        }

        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_EXIST; // 用户资料存在
    }
    else
    {
        sqlite3_finalize(stmt);
        return SQL_USER_PROFILE_NOT_EXIST; // 用户资料不存在
    }
}

int CreateUserProfile(const std::string &username, const ordered_json &data)
{
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }
    // 检查是否已经存在用户资料，如果存在则返回已存在用户资料，后续提示使用更新用户资料流程
    int ret = CheckUserProfileExists(username);
    if (ret == SQL_USER_PROFILE_EXIST)
    {
        sqlite3_close(db);
        std::cout << "User profile already exists. Please use the update user profile process." << std::endl;
        return SQL_USER_PROFILE_EXIST;
    }

    std::string sql = "INSERT INTO user_profiles (username, nickname, gender, birthday, bio, location, occupation, interests, education, website) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data["nickname"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, data["gender"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, data["birthday"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, data["bio"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, data["location"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, data["occupation"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, data["interests"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, data["education"].get<std::string>().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, data["website"].get<std::string>().c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to insert new user profile: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }
    return CREATE_USER_PROFILE_SUCCESS;
}

int UpdateUserProfileToDB(const std::string &username, const ordered_json &_data)
{
    sqlite3 *db;
    const char *dbPath = R"(..\..\db\user.db)";

    // 打开数据库连接
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return SQL_ERROR;
    }

    // 检查用户资料是否存在
    int ret = CheckUserProfileExists(username);
    if (ret == SQL_USER_PROFILE_NOT_EXIST)
    {
        sqlite3_close(db);
        return SQL_USER_PROFILE_NOT_EXIST;
    }

    // 获取表的列名和数据类型
    std::unordered_map<std::string, std::string> columnTypeMap;
    std::string sql = "PRAGMA table_info(user_profiles);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string columnName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        std::string columnType = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        columnTypeMap[columnName] = columnType;
    }
    sqlite3_finalize(stmt);

    // 更新用户资料
    sql = "UPDATE user_profiles SET ";
    bool first = true;
    for (auto &item : _data.items())
    {
        const std::string &key = item.key();
        const std::string &value = item.value();

        // 检查key是否在表中
        if (columnTypeMap.find(key) == columnTypeMap.end())
            continue;

        // 检查类型是否匹配
        if (columnTypeMap[key] == "TEXT" && !item.value().is_string())
            continue;
        if (columnTypeMap[key] == "INTEGER" && !item.value().is_number_integer())
            continue;

        if (!first)
            sql += ", ";
        sql += key + " = ?";
        first = false;
    }
    sql += " WHERE username = ?;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return SQL_ERROR;
    }

    int index = 1;
    for (auto &item : _data.items())
    {
        const std::string &key = item.key();
        const std::string &value = item.value();

        if (columnTypeMap.find(key) == columnTypeMap.end())
            continue;

        if (columnTypeMap[key] == "TEXT")
            sqlite3_bind_text(stmt, index++, value.c_str(), -1, SQLITE_STATIC);
        else if (columnTypeMap[key] == "INTEGER")
            sqlite3_bind_int(stmt, index++, item.value().get<int>());
    }
    sqlite3_bind_text(stmt, index, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to update user profile: " << sqlite3_errmsg(db) << std::endl;
        return UPDATE_USER_PROFILE_FAILURE;
    }

    return UPDATE_USER_PROFILE_SUCCESS;
}


int DealWithMessage(const std::string &ss, SOCKET clientSocket)
{
    std::cout << "INFO|DealWithMessage" << '\n';
    ordered_json j = ordered_json::parse(ss);
    std::cout << j.dump(4) << '\n';
    std::string cipher = j["cipher"];
    std::string type = j["type"];
    if (cipher != CIPHER)
        return CIPHER_ERROR;

    std::string username = j["username"];
    std::string password = j["password"];
    if (REQ_REGISTER == type) // 处理注册请求
    {
        // 注册用户,查询数据库，判断用户是否存在
        // 1. 存在，返回注册失败               status = STATUS_FAILURE
        // 2. 不存在，插入数据，返回注册成功    status = STATUS_SUCCESS
        // 3. 其他错误                        status = STATUS_UNKNOWN_ERROR
        int _ret = RegisterUser(username, password);
        if (REGISTER_SUCCESS == _ret)
        {
            std::cout << "INFO| Register User Success [" << username << "]" << std::endl;
            ordered_json _js = createSystemOrdJsonMessage();
            SetOrdJsonKV(_js, std::make_pair("cipher", CIPHER));
            SetOrdJsonKV(_js, std::make_pair("type", ANS_REGISTER));
            SetOrdJsonKV(_js, std::make_pair("username", username));
            SetOrdJsonKV(_js, std::make_pair("message", "Register success"));
            SetOrdJsonKV(_js, std::make_pair("status", STATUS_SUCCESS));
            std::string res_mes = _js.dump();
            std::cout << "INFO| return user profile 发送注册应答消息:" << "\n"
                      << _js.dump(4) << std::endl;
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return REGISTER_SUCCESS;
        }
        else if (SQL_USER_EXIST == _ret)
        {
            std::cout << "INFO| Register User Failed [" << username << "], User Exist" << std::endl;
            ordered_json _js = createSystemOrdJsonMessage();
            SetOrdJsonKV(_js, std::make_pair("cipher", CIPHER));
            SetOrdJsonKV(_js, std::make_pair("type", ANS_REGISTER));
            SetOrdJsonKV(_js, std::make_pair("username", username));
            SetOrdJsonKV(_js, std::make_pair("message", "Register failed, User Exist"));
            SetOrdJsonKV(_js, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = _js.dump();
            std::cout << "INFO| return user profile 发送注册应答消息:" << "\n"
                      << _js.dump(4) << std::endl;
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return REGISTER_FAILURE;
        }
        else
        {
            std::cout << "INFO| Register User Failed [" << username << "], Unknown Reason" << std::endl;
            ordered_json _js = createSystemOrdJsonMessage();
            SetOrdJsonKV(_js, std::make_pair("cipher", CIPHER));
            SetOrdJsonKV(_js, std::make_pair("type", ANS_REGISTER));
            SetOrdJsonKV(_js, std::make_pair("username", username));
            SetOrdJsonKV(_js, std::make_pair("message", "Register failed, Unknown Reason"));
            SetOrdJsonKV(_js, std::make_pair("status", STATUS_UNKNOWN_ERROR));
            std::string res_mes = _js.dump();
            std::cout << "INFO| return user profile 发送注册应答消息:" << "\n"
                      << _js.dump(4) << std::endl;
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return REGISTER_FAILURE;
        }
    }
    else if (REQ_LOGIN == type) // 处理登录请求
    {
        ordered_json j = createSystemOrdJsonMessage(); // 预组装返回消息
        SetOrdJsonKV(j, std::make_pair("cipher", CIPHER));
        SetOrdJsonKV(j, std::make_pair("type", ANS_LOGIN));
        SetOrdJsonKV(j, std::make_pair("username", username));
        // 查询数据库
        int _uid = QueryLoginInUser(username, password);
        if (0 > _uid)
        { // 登录失败
            std::cout << "INFO| Load User Account From DB [Failed] [" << username << "]" << std::endl;
            SetOrdJsonKV(j, std::make_pair("message", "Login failed"));
            SetOrdJsonKV(j, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = j.dump();
            std::cout << "INFO| return user profile 发送登录失败应答消息:" << "\n"
                      << j.dump(4) << std::endl;
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            return LOGIN_FAILURE;
        }
        else // 登录成功
        {
            // TODO 更新登录时间 (预计放到客户端去进行请求，或者在服务端登录校对过程中进行更新（也就是这里）)

            // 返回登录成功消息
            std::cout << "INFO| Load User Account From DB [Success] [" << username << "]" << std::endl;

            SetOrdJsonKV(j, std::make_pair("message", "Login success"));
            SetOrdJsonKV(j, std::make_pair("status", STATUS_SUCCESS));
            j["data"]["user_id"] = _uid;
            std::string res_mes = j.dump();
            std::cout << "INFO| return user profile 发送登录成功应答消息:" << "\n"
                      << j.dump(4) << std::endl;
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            // 添加一系列映射
            std::cout << "INFO| 添加映射 [" << username << "] -> [" << clientSocket << "]" << std::endl;
            AddUnameAndSocketMap(username, clientSocket); // 添加用户与套接字的映射
            username2userid_map[username] = _uid;
            userid2username_map[_uid] = username;

            return LOGIN_SUCCESS;
        }
    }
    else if (REQ_USER_PROFILE == type) // 处理请求用户资料
    {

        // 查询user_profiles表
        ordered_json _js = ordered_json::object();
        int _ret_qup = GetUserProfile(username, _js); // 查询并拉取用户资料
        ordered_json _ojs = createSystemOrdJsonMessage();
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
            std::cout << "INFO| Send Message to Return User Profile" << "\n"
                      << _ojs.dump(4) << std::endl;
            return SQL_USER_PROFILE_EXIST;
        }
        else
        {
            // 发送消息，用户资料不存在, 提示创建用户资料
            std::cout << "INFO| Load Profile From DB Failed [" << username << "], Please Create Profile ..." << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Remind User to Create Profile" << "\n"
                      << _ojs.dump(4) << std::endl;
            return SQL_USER_PROFILE_NOT_EXIST;
        }
    }
    else if (REQ_CREATE_USER_PROFILE == type)
    {
        // 创建用户资料,插入到表中
        ordered_json _data = j["data"];
        int _ret = CreateUserProfile(username, _data);
        ordered_json _ojs = createSystemOrdJsonMessage(CIPHER, ANS_CREATE_USER_PROFILE, username);
        SetOrdJsonKV(_ojs, std::make_pair("data", _data)); // 把提交的资料再原样返回
        if (CREATE_USER_PROFILE_SUCCESS == _ret)
        {
            std::cout << "INFO| Create Profile Success [" << username << "]" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_SUCCESS));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Return Create User Profile Success" << "\n"
                      << _ojs.dump(4) << std::endl;
            return CREATE_USER_PROFILE_SUCCESS;
        }
        else if (SQL_USER_PROFILE_EXIST == _ret)
        {
            std::cout << "INFO| Create Profile Failed [" << username << "], Profile Exist" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Run Update User Profile" << "\n"
                      << _ojs.dump(4) << std::endl;
            return CREATE_USER_PROFILE_FAILURE;
        }
        else
        {
            std::cout << "INFO| Create Profile Failed [" << username << "]" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_ERROR));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Return Create User Profile Failure" << "\n"
                      << _ojs.dump(4) << std::endl;
            return CREATE_USER_PROFILE_FAILURE;
        }
    }
    else if (REQ_UPDATE_USER_PROFILE == type) // 处理更新用户资料请求
    {
        // 更新用户资料
        ordered_json _data = j["data"];
        int _ret = UpdateUserProfileToDB(username, _data);
        ordered_json _ojs = createSystemOrdJsonMessage(CIPHER, ANS_UPDATE_USER_PROFILE, username);

        if (UPDATE_USER_PROFILE_SUCCESS == _ret)
        {
            std::cout << "INFO| Update Profile Success [" << username << "]" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_SUCCESS));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Return Update User Profile Success" << "\n"
                      << _ojs.dump(4) << std::endl;
            return UPDATE_USER_PROFILE_SUCCESS;
        }
        else if (SQL_USER_PROFILE_NOT_EXIST == _ret)
        {
            std::cout << "INFO| Update Profile Failed [" << username << "], Profile Not Exist" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_FAILURE));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Run Create User Profile" << "\n"
                      << _ojs.dump(4) << std::endl;
            return UPDATE_USER_PROFILE_FAILURE;
        }
        else // (UPDATE_USER_PROFILE_FAILURE == _ret) or (SQL_ERROR == _ret) 数据库问题或其他问题
        {
            std::cout << "INFO| Update Profile Failed [" << username << "]" << std::endl;
            SetOrdJsonKV(_ojs, std::make_pair("status", STATUS_ERROR));
            std::string res_mes = _ojs.dump();
            send(clientSocket, res_mes.c_str(), res_mes.size(), 0);
            std::cout << "INFO| Send Message to Return Update User Profile Failure" << "\n"
                      << _ojs.dump(4) << std::endl;
            return UPDATE_USER_PROFILE_FAILURE;
        }
        return DEFAULT_ERROR;
    }
    else if (REQ_LOGOUT == type) // 处理登出请求
    {
        // 返回消息可能不需要 TODO~
        // 移除用户的在线映射
        std::cout << "INFO|用户登出，移除相关映射关系 " << std::endl;
        std::cout << "INFO| Remove User [" << username << "] From Online Map" << std::endl;
        RemoveUnameAndSocketMap(username);
        // 用户名 和 ID 好像不需要移除，否则会对其他人发来消息性能有影响
        return LOGOUT_SUCCESS;
    }
    else if (REQ_SEND_MESSAGE == type) // 处理发送消息请求
    {
        // TODO***  处理发送消息请求
        std::string targetUser = j["targetUser"];
        std::string message = j["message"];
        SOCKET targetSocket = GetUserSocket(targetUser);
        if (targetSocket != INVALID_SOCKET)
        {
            send(targetSocket, message.c_str(), message.size(), 0);
            std::cout << "INFO| Message sent to [" << targetUser << "]" << std::endl;
        }
        else
        {
            std::cout << "ERROR| Target user [" << targetUser << "] not online" << std::endl;
        }
    }
    else if (REQ_CHAT == type) // 处理聊天请求
    {
    }
    else if (REQ_GROUP == type) // 处理群组请求
    {
    }
    else if (REQ_FRIEND == type) // 处理好友请求
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

        try
        {
            DealWithMessage(buffer, clientSocket);
        }
        catch (const std::exception &e)
        {
            std::cerr << "处理消息时发生异常: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "处理消息时发生未知异常" << std::endl;
        }

        // 复读机
        // std::string str = std::string(buffer);
        // send(clientSocket, str.c_str(), str.size(), 0);
    }
    std::cout << clientSocket << "退出服务器" << std::endl;
    closesocket(clientSocket);

    return 0;
}

int main()
{
    // 1. 打开数据库
    sqlite3 *user_db = nullptr;
    int ret_db = OpenDB(user_db, R"(..\..\db\user.db)"); // 账户表
    if (OPEN_DB_SUCCESS != ret_db)
        return -1; // 读取失败
    sqlite3 *profile_db = nullptr;
    ret_db = OpenDB(profile_db, R"(..\..\db\profile.db)"); // 用户资料表
    if (OPEN_DB_SUCCESS != ret_db)
        return -1; // 读取失败

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

/*

chcp 65001

g++ -o server server.cpp -lws2_32
g++ -o server server.cpp -lws2_32 -lsqlite3

g++ server.cpp  ../tool/tool.cpp ../tool/jsontool.cpp ../../chat/user/user.cpp  -o server.exe -I include -L . -l sqlite3 -lws2_32 -lrpcrt4 # 之后编译只要这步即可

*/