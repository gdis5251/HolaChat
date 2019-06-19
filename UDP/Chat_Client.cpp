#include "udp_client.hpp"
#include <thread>
#include <functional>

void menu()
{
    std::cout << "*********************************" << std::endl;
    std::cout << "********   a. 通讯录   **********" << std::endl;
    std::cout << "********   q. 退  出   **********" << std::endl;
    std::cout << "*********************************" << std::endl;
}

struct ThreadArgv
{
    ThreadArgv()
        :client_("47.101.192.120", 9090)
    {}
    UdpClient client_;
    std::string resp_;
};

void* RecvMessage(void *argv)
{
    while (true)
    {
        (*(ThreadArgv*)argv).client_.RecvFrom((*(ThreadArgv*)argv).resp_);
        std::cout << "> " << (*(ThreadArgv*)argv).resp_.c_str() << std::endl;
    }

    return nullptr;
}

int main(void)
{
    UdpClient client("47.101.192.120", 9090); // 这里我把服务器的地址和端口号写死了
    std::string resp;
    
	// 1.客户端启动，服务器记录客户
	std::cout << "请输入您的姓名：";
    std::string start("__client__start ");
    std::string name;
    std::cin >> name;

    // 首先在客户端启动的时候获取到用户的 ip 端口号 姓名
    start += name;
    // 先给服务器发送一句话，让服务器记录下来该用户
    client.SendTo(start);

    // 这里弄一个线程专门去接收消息
    // std::thread recv(RecvMessage, client, resp);
    // recv.join();
    
    ThreadArgv argv;
    argv.client_ = client;
    argv.resp_ = resp;

    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, RecvMessage, (void*)&argv);



    while(true)
    {
        menu();
        
        std::cout << "请输入您的选项：";
        fflush(stdout);

        char option = 0;
        std::cin >> option;
        std::string msg;
        

        if (option == 'a')
        {
            msg = "__client__list ";
            client.SendTo(msg);

            // 看完列表后，选择要发送的用户
            std::cout << "请选择要发送的消息的用户：";
            std::string ans("__client__message ");
            std::string username;
            std::cin >> username;

            std::cout << "< ";
            std::string message;
            getchar();
            std::getline(std::cin, message);

            ans += username;
            ans += ' ';
            ans += message;
            client.SendTo(ans);
        }
        else if (option == 'q')
        {
            // 用户退出
            msg = "__client__quit ";
            client.SendTo(msg);
            return 1;
        }
    }


    return 0;
}


