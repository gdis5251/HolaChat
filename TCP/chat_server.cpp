#include <map>
#include <sstream>
#include "tcp_thread_server.hpp"

int main()
{
    // 1. 创建 socket 文件描述符
    //   并创建map用来记录用户信息
    TcpThreadServer server;
    
    std::map<std::string, TcpSocket> book;

    // 2. 启动服务器
    server.start("0", 9090, [&book](const std::string req, std::string* resp,
                                    TcpSocket& sock)
                 {
                    // 1. 截取出请求的第一个空格前的信息
                    std::stringstream ss(req);
                    std::string option;
                    ss >> option;

                    // 2. 根据信息内容分类处理
                    //  a) 如果用户是第一次连接，就记录用户信息，并返回连接成功
                    if (option == "client_start")
                    {
                        std::string name;
                        ss >> name;
                        book.insert(std::make_pair(name, sock));

                        *resp = "Connect Success!";
                    }
                    //  b) 如果用户需要查看通讯录，把通讯录列出来
                    else if (option == "client_list")
                    {
                        for (const auto& e : book)
                        {
                            *resp += e.first;
                            *resp += " ";
                        }
                    }
                    //  c) 如果用户想要发送消息
                    else if (option == "client_send")
                    {
                    //      在通讯录中查找用户想要发消息的用户
                        std::string peer_name;
                        ss >> peer_name;
                        auto it = book.find(peer_name);
                    //      如果找到，给对方发消息，并返回发送成功
                        if (it != book.end())
                        {
                            std::string msg;
                            int pos = req.find(' ');
                            pos = req.find(' ', pos + 1);

                            msg = req.substr(pos + 1);

                            // TcpSocket send_sock;
                            it->second.Send(msg);
                            

                            *resp = "Send Success!";
                        }
                    //      如果没找到，返回没找到
                        else
                        {
                            *resp = "Didn't find user!";
                        }
                    }
                    //  d) 如果用户退出，就把用户在通讯录上的信息删除掉
                    else if (option == "client_quit")
                    {
                        for (auto& e : book)
                        {
                            if (&(e.second) == &sock)
                            {
                                std::cout << "delete" << std::endl;
                                auto it = book.find(e.first);
                                book.erase(it);
                                break;
                            }
                        }
                    }
                 }
                 ); 
    return 0;
}
