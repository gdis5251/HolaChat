#include <pthread.h>
#include "tcp_client.hpp"

pthread_mutex_t mutex;
pthread_cond_t cond;

void menu()
{
    std::cout << "*********************************" << std::endl;
    std::cout << "*******   1. 查看通讯录   *******" << std::endl;
    std::cout << "*******   2. 退出  程序   *******" << std::endl;
    std::cout << "*********************************" << std::endl;
}

void* ThreadEntry(void *arg)
{
    TcpClient* client = (TcpClient*)arg;

    while (true)
    {
        std::string resp;
        client->Recv(&resp);

        std::cout << resp.c_str() << std::endl;
        pthread_cond_signal(&cond);
    }


    return nullptr;
}

int main(void)
{
    TcpClient client;
    client.Connect();

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    // 客户端一启动，就给服务器发送一个自己名字的消息
    // 让服务器记录该用户
    std::string start = "client_start ";
    std::string init_name;
    std::cout << "请输入用户名-> ";
    fflush(stdout);
    std::cin >> init_name;
    start += init_name;
    client.Send(start);

    std::string resp;
    client.Recv(&resp);
    std::cout << resp.c_str() << std::endl;

    // 创建一个线程用来专门接收消息
    pthread_t tid;
    int ret = pthread_create(&tid, nullptr, ThreadEntry, (void*)&client);
    if (ret < 0)
    {
        perror("pthread_create");
        return 1;
    }
    pthread_detach(tid);
    
    while (true)
    {
        // 1. 先让用户选择是查看通讯录还是退出程序
        menu();
        std::cout << "请选择您想要选择的选项：";
        fflush(stdout);
        int option = 0;
        std::cin >> option;
        // 2. 如果用户选择查看通讯录
        if (option == 1)
        {
            //    就列出当前连接的所有用户名
            std::string req_list("client_list");
            client.Send(req_list);


            std::cout << "在线的用户为-> ";
            fflush(stdout);
            pthread_cond_wait(&cond, &mutex);
            //    用户通过先选择用户名
            std::string req_msg("client_send ");
            std::cout << "请输入您要聊天的用户名：";
            // fflush(stdout);
            std::string name;
            std::cin >> name;
            req_msg += name;
            req_msg += ' ';

            // 用getchar接收一下输入名字时输入的回车
            getchar();
            std::cout << "< ";
            // fflush(stdout);
            std::string msg;
            std::getline(std::cin, msg);
            req_msg += msg;
            
            //    再编译发送内容的方式实现通讯
            client.Send(req_msg);
            pthread_cond_wait(&cond, &mutex);
        }
        // 3. 如果用户选择退出程序
        else if (option == 2)
        {
            //    就给服务器发送用户选择退出
            //    服务器删除该用户信息
            std::string req_msg("client_quit ");
            req_msg += init_name;

            client.Send(req_msg);
            break;
        }
        else 
        {
            std::cout << "输入出错，请重新输入！" << std::endl;
        }
    }


    return 0;
}
