#include "udp_server.hpp"
#include <vector>
#include <map>

struct AddressList
{
    // 让姓名成为 key，然后在 value 里存 ip 和 端口号
    std::map<const std::string, std::pair<const std::string, uint16_t> > book;
};


void AddUser(const std::string& name, const std::string& ip, uint16_t port, AddressList& addresslist)
{
    addresslist.book.insert(std::make_pair(name, std::make_pair(ip, port)));
}

void DeleteUser(const std::string& name, AddressList& addresslist)
{
    addresslist.book.erase(addresslist.book.find(name));
    std::cout << "Success Delete" << std::endl;
}

int main(void)
{
    AddressList addresslist;

    UdpServer server;

    server.start("0", 9090, [&addresslist](const std::string& req, std::string& resp,
                                           const std::string& ip, uint16_t port)
                 {
                    // 先判断是否是程序刚启动
                    int pos = req.find(' ');
                    if (pos != -1)
                    {
                        std::string start = req.substr(0, pos);

                        std::string name = req.substr(pos + 1);
                        
                        // 如果是程序刚启动，就先记录该用户信息
                        if (start == "__client__start")
                        {
                            std::cout << "Add User" << std::endl;
                            AddUser(name, ip, port, addresslist);
                        }
                        else if (start == "__client__quit")
                        {
                            std::cout << "Delete User" << std::endl;
                            DeleteUser(name, addresslist);
                        }
                        return;
                    }

                    // 在这里处理用户发的消息

                 }
                );
    return 0;
}
