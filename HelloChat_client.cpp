#include "udp_client.hpp"

void menu()
{
    std::cout << "*********************************" << std::endl;
    std::cout << "********   a. 通讯录   **********" << std::endl;
    std::cout << "*********************************" << std::endl;
}

int main(void)
{
    UdpClient client("43.101.192.120", 9090); // 这里我把服务器的地址和端口号写死了
    std::cout << "请输入您的姓名：";
    std::string start("__client__start ");

    std::string input;
    std::cin >> input;

    // 首先在客户端启动的时候获取到用户的 ip 端口号 姓名
    start += input;
    // 先给服务器发送一句话，让服务器记录下来该用户
    client.SendTo(start);

    while(true)
    {
        menu();
        std::cout << "请输入您：";
        fflush(stdout);
        int num = 0;
        std::cin >> num;

        // client.chat(num);
    }
}


