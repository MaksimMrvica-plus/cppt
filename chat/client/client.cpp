#include "client.h"

// TODO 全局变量, 本地资料准备
SOCKET clientSocket;
USER user;
UserProfile user_profile;
MessageQueue<std::string> sendQueue;
MessageQueue<std::string> recvQueue;
std::unordered_map<std::string, u_int64> user2id;
std::unordered_map<std::string, u_int64> TEST_USED_user2id = {{"admin", 1},{"alllallll",9},{"test3",666}};
std::unordered_set<std::string> TEST_USED_friend_username_uord_set = {"admin","alllallll","test1","test2"};


int main()
{

    // 1. 初始化客户端socket
    clientSocket = InitializeClientSocket();
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "Failed to initialize client socket, exiting program..." << '\n';
        return SOCKET_ERROR;
    }


    // // 3. 加载用户资料到本地
    // // 发送资料请求消息
    // ordered_json req_profile = createSystemOrdJsonMessage(CIPHER, "R_PROFILE", user.getUsername());
    // std::cout << "INFO | 发送请求用户资料消息:\n"
    //           << req_profile.dump(4) << '\n';
    // std::string profile_mes = req_profile.dump();
    // send(clientSocket, profile_mes.c_str(), profile_mes.size(), 0);
    // // 接收返回结果, 包含用户资料
    // std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    // int ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    // if (ret <= 0)
    // {
    //     std::cout << "ERROR|Failed to receive the user profile message procedure" << '\n';
    //     return -1;
    // }
    // // 处理返回用户资料
    // int res_code = DealWithMessage(rbuffer); // 如果成功，在函数内已加载到内存
    // if (LOAD_USER_PROFILE_FAILURE == DealWithMessage(rbuffer))
    // {
    //     std::cout << "是否创建用户资料?" << std::endl;
    //     std::string choice;
    //     std::getline(std::cin, choice);
    //     if (choice == "n" or choice == "N")
    //     {
    //         std::cout << "取消创建用户资料，跳过此步骤..." << std::endl;
    //     }
    //     else if (choice == "y" or choice == "Y")
    //     {
    //         // 创建用户资料流程
    //         // 1. 创建用户资料
    //         ordered_json ojs = {};
    //         int ret = InputUserProfile(ojs);
    //         if (SUCCESS == ret)
    //         {
    //             // 2. 发送创建用户资料消息
    //             ordered_json _j = createSystemOrdJsonMessage(CIPHER, REQ_CREATE_USER_PROFILE, user.getUsername());
    //             SetOrdJsonKV(_j, std::make_pair("data", ojs));
    //             std::string send_mes = _j.dump();
    //             send(clientSocket, send_mes.c_str(), send_mes.size(), 0);
    //             std::cout << "INFO|Send a message to create user profile -->>" << '\n'
    //                       << _j.dump(4) << '\n';
    //             // 3. 接收返回结果
    //             std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    //             int r_ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    //             if (r_ret <= 0)
    //             {
    //                 std::cout << "ERROR|Failed to receive the create user profile message procedure" << '\n';
    //                 return -1;
    //             }
    //             // 处理返回用户资料, 加载到内存
    //             if (CREATE_USER_PROFILE_SUCCESS == DealWithMessage(rbuffer))
    //             {
    //                 std::cout << "INFO|Create User Profile Success !" << '\n';
    //                 std::cout << "INFO|Loading User Profile ..." << '\n';
    //                 ordered_json _data = ordered_json::parse(rbuffer)["data"];
    //                 updateUserProfile(user_profile, _data);
    //             }
    //             else
    //             {
    //                 std::cout << "ERROR|创建用户资料失败，跳过此步骤... !" << '\n';
    //             }
    //         }
    //     }
    //     else{
    //         std::cout << "输入错误，跳过此步骤... !" << std::endl;
    //     }
    // }
    // std::cout << "当前用户最新资料" << std::endl;
    // user_profile.displayUserProfile();

    // // TODO*** 4. 聊天功能
    // ordered_json input_content = getMutipleUserInputJson();
    // std::cout << input_content.dump(4) << std::endl;
    // //
    // // 组装data部分
    // ordered_json data = createSendMessageJson(generate_uuid(), user.getIntUserID(), 0);
    // SetOrdJsonKV(data, std::make_pair("content", input_content));
    // ordered_json sys_mes = createSystemOrdJsonMessage(CIPHER, REQ_SEND_MESSAGE, user.getUsername(), "", data);
    // std::string send_mes = sys_mes.dump();
    // send(clientSocket, send_mes.c_str(), send_mes.size(), 0);
    // std::cout << "INFO|发送聊天消息 -->>" << '\n'
    //           << sys_mes.dump(4) << '\n';
    
    // 多线程，发送和接收
    // Create send and receive threads
    std::thread send_thread(SendThread, clientSocket);
    std::thread receive_thread(ReceiveThread, clientSocket);
    std::thread process_thread(ProcessThread, clientSocket);
    while (1){ // 进入用户操作循环
        // 等待1s延迟
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // 1 询问用户选择操作，
        int opt = ChooseOperation();
        // 2 进入相应流程,匹配对应消息
        int ret = DealWithOperation(opt, clientSocket);
        if (CHOOSE_OPERATION_ERROR == ret)
        {
            std::cout << "Choose Invalid Operation" << '\n';
            continue;
        }

    }

    // Wait for threads to finish
    send_thread.join();
    receive_thread.join();
    process_thread.join();
    // 3 send
    // while (1)
    // {
    //     std::string sbuffer = {0};
    //     GetInputString(sbuffer, MAX_MESSAGE_LENGTH);
    //     send(clientSocket, sbuffer.c_str(), sbuffer.size(), 0);
    //     std::cout << "send success success !" << '\n';

    //     std::string rbuffer(MESSAGE_LENGTH_1K, '\0'); // 分配足够的空间
    //     int ret = recv(clientSocket, &rbuffer[0], MESSAGE_LENGTH_1K, 0);
    //     if (ret <= 0)
    //         break;
    //     std::cout << "receive message:" << '\n'
    //               << rbuffer << std::endl;
    // }

    // 4 close
    closesocket(clientSocket);

    return 0;
}

/*

g++ client.cpp -o client.exe -L ../server  -l sqlite3
g++ -o client client.cpp -lws2_32
g++ client.cpp -o client.exe -I include -L . -l sqlite3 -lws2_32
要带着其他自定义库文件一起编译

g++ -o client client.cpp ../tool/tool.cpp ../tool/clitool.cpp ../tool/jsontool.cpp ../../chat/user/user.cpp ../../chat/user/userprofile.cpp -lws2_32 -lrpcrt4


*/