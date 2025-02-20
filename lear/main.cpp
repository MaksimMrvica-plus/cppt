#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
    u_short hostPort = 8080;  // 主机字节序的端口号
    u_short netPort = htons(hostPort);  // 转换为网络字节序

    std::cout << "Host Port: " << hostPort << std::endl;
    std::cout << "Network Port: " << netPort << std::endl;

    return 0;
}