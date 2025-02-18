#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <string>

// 定义消息最大长度（根据协议需要）
const int MAX_MESSAGE_LENGTH = 10;
const int MESSAGE_LENGTH_1K = 1024;
const int MESSAGE_LENGTH_2K = 2048;
const int MESSAGE_LENGTH_4K = 4096;
const int MESSAGE_LENGTH_8K = 8192;
const std::string CIPHER        =  "[NXFZZB!Q#3SMB]";
const std::string CIPHER_STR    =    "NXFZZB!Q#3SMB";

// 消息返回码
#define DEFAULT_ERROR            -999
#define CIPHER_ERROR                0
#define REGISTER_SUCCESS            7
#define REGISTER_FAILURE            8
#define LOGIN_SUCCESS               9
#define LOGIN_FAILURE              10
#define LOGIN_QUIT                 11

#define SOCKET_ERROR               -8

// 数据库返回码
#define OPEN_DB_SUCCESS             0
#define OPEN_DB_FAILURE            -1
#define OPEN_DB_ERROR              -2


// SQL账号查询返回码
#define SQL_ERROR                  -1
#define SQL_USER_EXIST              1
#define SQL_USER_NOT_EXIST          2
#define SQL_USER_PASSWORD_ERROR     3
#define SQL_USER_PASSWORD_CORRECT   4





#endif