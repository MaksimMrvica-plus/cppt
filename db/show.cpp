#include <iostream>
#include <string>
#include "D:\cppprojects\cppt\chat\client\define.h"
#include "D:/cppprojects/cppt/chat/server/sqlite3.h"

int OpenDB(sqlite3*& db) {
    // 打开数据库
    std::string db_path = R"(D:\cppprojects\cppt\db\user.db)";
    int rc = sqlite3_open(db_path.c_str(), &db);  // 使用正斜杠作为路径分隔符
    std::cout << rc << std::endl;
    if (rc != SQLITE_OK) {
        if (db) {  // 确保 db 不是 nullptr
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);  // 关闭数据库
        } else {
            std::cerr << "Cannot open database: sqlite3_open failed to initialize db pointer." << std::endl;
        }
        return -1;
    }
    else {
        std::cout << "Open database successfully :\n -> \t [ " << db_path << " ]" << std::endl;
    }
    return 0;
}

void ShowUsers(sqlite3* db) {
    std::string sql = "SELECT * FROM users;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    int column_count = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < column_count; ++i) {
            const char* column_name = sqlite3_column_name(stmt, i);
            const char* column_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            std::cout << column_name << ": " << (column_value ? column_value : "NULL") << "\t";
        }
        std::cout << std::endl;
    }

    sqlite3_finalize(stmt);
}

int CheckUser(sqlite3* db, const std::string& username, const std::string& password) {
    std::string sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char* db_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (db_password && password == db_password) {
            sqlite3_finalize(stmt);
            return SQL_USER_PASSWORD_CORRECT; // 用户名和密码正确
        } else {
            sqlite3_finalize(stmt);
            return SQL_USER_PASSWORD_ERROR; // 密码错误
        }
    } else {
        sqlite3_finalize(stmt);
        return SQL_USER_NOT_EXIST; // 用户不存在
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    sqlite3* db = nullptr;
    int ret_db = OpenDB(db);
    if (ret_db == -1) {
        return -1; // 读取失败
    }

    ShowUsers(db);

    std::string username = "JohneDeeoe";
    std::string password = "sa2jfi@aQ123";
    int result = CheckUser(db, username, password);
    if (result == 1) {
        std::cout << "User exists and password is correct." << std::endl;
    } else if (result == 0) {
        std::cout << "User exists but password is incorrect." << std::endl;
    } else {
        std::cout << "User does not exist." << std::endl;
    }

    sqlite3_close(db);
    return 0;
}


// 下面两个都可以编译通过
// g++ show.cpp -o show.exe -L ../chat/server  -l sqlite3 # 之后编译只要这步即可
// g++ -g show.cpp -o show.exe -I "D:/cppprojects/cppt/chat/server" -L "D:/cppprojects/cppt/chat/server" -lsqlite3

