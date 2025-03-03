#include "client.h"

// TODO 全局变量, 本地资料准备
SOCKET clientSocket;
USER user;
UserProfile user_profile;
MessageQueue<std::string> sendQueue;
MessageQueue<std::string> recvQueue;
std::unordered_map<std::string, u_int64> user2id;
std::unordered_map<std::string, u_int64> TEST_USED_user2id = {{"admin", 1},{"alllallll",9},{"test3",666}};
std::unordered_set<std::string> TEST_USED_friend_username_uord_set = {"admin","alllallll","test1","test2","apt"};




int main()
{
    // 1. 初始化客户端socket
    clientSocket = InitializeClientSocket();
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "Failed to initialize client socket, exiting program..." << '\n';
        return SOCKET_ERROR;
    }

    // 多线程，发送和接收
    std::thread send_thread(SendThread, clientSocket);
    std::thread receive_thread(ReceiveThread, clientSocket);
    std::thread process_thread(ProcessThread, clientSocket);

    while (true) // 进入用户操作循环
    {
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