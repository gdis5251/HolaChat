#include "udp_server.hpp"
#include <vector>


struct UserInfo
{
    std::string name;
    std::string ip;
    uint16_t port;
};

struct AddressList
{
    std::vector<UserInfo*> book;
};

// 创建一个全局的通讯录
AddressList addresslist;

void AddUser(const std::string& name, const std::string& ip, uint16_t port)
{
    UserInfo *user = new UserInfo;
    user->name = name;
    user->ip = ip;
    user->port = port;

    addresslist.book.push_back(user);
}

struct HelloChatHandler
{
    void operator()(const std::string& req, std::string& resp,
                    const std::string& ip, uint16_t port)
    {
        // 先判断是否是程序刚启动
        int pos = req.find(' ');
        if (pos != -1)
        {
            std::string start = req.substr(0, pos);
            
            // 如果是程序刚启动，就先记录该用户信息
            if (start == "__client__start")
            {
                std::string name = req.substr(pos + 1);
                AddUser(name, ip, port);

                // 测试是否插入成功
                std::cout << addresslist.book.back()->name.c_str() << std::endl;
                resp = "connected success!";
            }
            return;
        }

        // 在这里处理用户发的消息

    }
};

int main(void)
{
    UdpServer server;
    HelloChatHandler handler;

    server.start("0", 9090, handler);

    return 0;
}
