#pragma once
#include <cassert>
#include <functional>
#include "udp_socket_api.hpp"

// 一个可以接收函数指针和仿函数的通用的东西
typedef std::function<void(const std::string&, std::string&, 
                           const std::string&, uint16_t,
                           std::string&, uint16_t*, int*, std::string&)> Handler;

class UdpServer
{
public:
    UdpServer() 
    {
        sock_.Socket();
    }
    ~UdpServer() 
    {
        sock_.Close();
    }

    bool start(const std::string& ip, uint16_t port, Handler handler)
    {
        // 先绑定
        int ret = sock_.Bind(ip, port);
        if (!ret)
        {
            perror("Bind");
            return false;
        }
        
        while (true)
        {
            std::string req;
            std::string peer_ip;
            uint16_t peer_port;
            // 1. 接收请求
            ret = sock_.RecvFrom(req, &peer_ip, &peer_port);
            if (!ret)
            {
                perror("RecvFrom");
                continue;
            }

            // 2. 处理请求
            std::string send_ip;
            uint16_t send_port;
            int is_send = 0;
            std::string msg;
            std::string resp;
            handler(req, resp, peer_ip, peer_port,
                    send_ip, &send_port, &is_send, msg);

            if (is_send == 1)
            {
                sock_.SendTo(msg, send_ip, send_port);
            }
            // 3. 响应请求
            sock_.SendTo(resp, peer_ip, peer_port);
        }
    }

private:
    UdpSocket sock_;
};

