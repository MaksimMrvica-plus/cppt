#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <string>

// 定义消息最大长度（根据协议需要）
const int MAX_MESSAGE_LENGTH = 10;
const int MESSAGE_LENGTH_1K = 1024;
const int MESSAGE_LENGTH_2K = 2048;
const int MESSAGE_LENGTH_4K = 4096;
const int MESSAGE_LENGTH_8K = 8192;
const std::string CIPHER            =    "NXFZZB!Q#3SMB";
const std::string CIPHER_SQUARE     =  "[NXFZZB!Q#3SMB]";

// 通用返回码
#define SUCCESS                    1
#define DEFAULT_SUCCESS            2

#define FAILURE                    0
#define DEFAULT_FAILURE           -1
#define DEFAULT_QUIT              -2
#define UNKNOWN_ERROR             -3

// 消息、状态 返回码
#define DEFAULT_ERROR            -999
#define CIPHER_ERROR             -998
#define REGISTER_SUCCESS            7
#define REGISTER_FAILURE           -7
#define LOGIN_SUCCESS              -9
#define LOGIN_FAILURE             -10
#define LOGIN_QUIT                -11
#define LOGIN_ADMIN_SUCCESS        12 
#define LOGIN_ADMIN_FAILURE       -12 
#define LOAD_USER_PROFILE_SUCCESS   8
#define LOAD_USER_PROFILE_FAILURE  -8
#define CREATE_USER_PROFILE_SUCCESS 9
#define CREATE_USER_PROFILE_FAILURE -9

// 数据库返回码
#define OPEN_DB_SUCCESS             1
#define OPEN_DB_FAILURE            -1
#define OPEN_DB_ERROR              -2


// SQL用户账号查询返回码
#define SQL_ERROR                  -1
#define SQL_USER_EXIST              1
#define SQL_USER_NOT_EXIST          2
#define SQL_USER_PASSWORD_ERROR    -3
#define SQL_USER_PASSWORD_CORRECT   4


#define SQL_USER_PROFILE_EXIST      6
#define SQL_USER_PROFILE_NOT_EXIST -6





// 请求 类型码
#define REQ_USER_PROFILE            "R_PROFILE"
#define REQ_REGISTER                "R_REGISTER"
#define REQ_LOGIN                   "R_LOGIN"
#define REQ_LOGOUT                  "R_LOGOUT"
#define REQ_CHAT                    "R_CHAT"
#define REQ_GROUP                   "R_GROUP"
#define REQ_FRIEND                  "R_FRIEND"
#define REQ_CREATE_USER_PROFILE     "R_CREATE_PROFILE"
#define REQ_UPDATE_USER_PROFILE     "R_UPDATE_PROFILE"
#define REQ_DELETE_USER_PROFILE     "R_DELETE_PROFILE"
#define REQ_SEND_MESSAGE            "R_SEND_MESSAGE"
#define REQ_RECEIVE_MESSAGE         "R_RECEIVE_MESSAGE"

// 响应 类型码
#define ANS_USER_PROFILE            "A_PROFILE"
#define ANS_REGISTER                "A_REGISTER"
#define ANS_LOGIN                   "A_LOGIN"
#define ANS_LOGOUT                  "A_LOGOUT"
#define ANS_CHAT                    "A_CHAT"
#define ANS_GROUP                   "A_GROUP"
#define ANS_FRIEND                  "A_FRIEND"
#define ANS_CREATE_USER_PROFILE     "A_CREATE_PROFILE"
#define ANS_UPDATE_USER_PROFILE     "A_UPDATE_PROFILE"
#define ANS_DELETE_USER_PROFILE     "A_DELETE_PROFILE"
#define ANS_SEND_MESSAGE            "A_SEND_MESSAGE"
#define ANS_RECEIVE_MESSAGE         "A_RECEIVE_MESSAGE"

// status 状态码
#define STATUS_SUCCESS              "SUCCESS"
#define STATUS_FAILURE              "FAILURE"
#define STATUS_ERROR                "ERROR"
#define STATUS_UNKNOWN_ERROR        "UNKNOWN_ERROR"


// 更新本地用户资料
#define UPDATE_PROFILE_SUCCESS      1
#define UPDATE_PROFILE_FAILURE      0


#endif