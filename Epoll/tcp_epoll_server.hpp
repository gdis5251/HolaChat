#pragma once
#include "tcp_socket.hpp"
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/epoll.h>
#include <pthread.h>

class Epoll
{
public:
    Epoll()
    {
        epoll_fd_ = epoll_create(1);
    }

    ~Epoll()
    {
        close(epoll_fd_);
    }

    void Add(const TcpSocket& sock)
    {
        epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = sock.GetFd();

        int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock.GetFd(), &event);
        if(ret < 0)
        {
            perror("epoll_ctl");
            return;
        }
    }

    void Del(const TcpSocket& sock)
    {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, sock.GetFd(), nullptr);
    }

    void Wait(std::vector<TcpSocket> *fd_list)
    {
        fd_list->clear();
        epoll_event events[100];

        int ret = epoll_wait(epoll_fd_, events, 100, -1);
        if (ret < 0)
        {
            perror("epoll_wait");
            return;
        }

        for (int i = 0; i < ret; i++)
        {
            fd_list->push_back(TcpSocket(events[i].data.fd));
        }
    }
private:
    int epoll_fd_; 
};

typedef std::function<void (const std::string& req, std::string* resp,
                            TcpSocket& send_sock)> Handler;

#define CHECK_RET(exp) if (!(exp)) {return false;}

class TcpEpollServer
{
public:
    TcpEpollServer()
    {

    }
    ~TcpEpollServer()
    {
        listen_sock_.Close();
    }

    bool start(const std::string&ip, uint16_t port, Handler handler)
    {
        CHECK_RET(listen_sock_.Socket());

        CHECK_RET(listen_sock_.Bind(ip, port));

        CHECK_RET(listen_sock_.Listen());

        // 创建一个 epoll 对象
        // 把监听描述符添加进去
        Epoll epoll;
        epoll.Add(listen_sock_);

        while (true)
        {
            // 1. 获取连接
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            std::vector<TcpSocket> fd_list;

            epoll.Wait(&fd_list);

            for (auto& tcp_sock : fd_list)
            {
                if (tcp_sock.GetFd() == listen_sock_.GetFd())
                {
                    int ret = listen_sock_.Accept(&client_sock, &peer_ip, &peer_port); 
                    if (ret < 0)
                    {
                        continue;
                    }
                    
                    printf("[client %s:%d] Connection\n", peer_ip.c_str(), peer_port);
                    epoll.Add(client_sock);
                }
                else
                {
                    // 1. 接收请求
                    std::string req;
                    int r = tcp_sock.Recv(&req);
                    if (r < 0)
                    {
                        // 接收失败
                        continue;
                    }
                    if (r == 0)
                    {
                        printf("对端关闭！\n");

                        req += "client_quit";
                        std::string fake_resp;
                        handler(req, &fake_resp, tcp_sock);

                        tcp_sock.Close();
                        epoll.Del(tcp_sock);
                        break;
                    }
                    // 走到这里说明接收消息成功
                    printf("对端发送了： %s\n", req.c_str());

                    // 2. 处理请求
                    std::string resp;
                    handler(req, &resp, tcp_sock);

                    // 3. 将响应结果发送回客户端
                    tcp_sock.Send(resp);

                } // end of else
            } // end of for (auto& tcp_sock : fd_list)
        } // end of while(true)
    } // end of start
private:
    TcpSocket listen_sock_;
};
