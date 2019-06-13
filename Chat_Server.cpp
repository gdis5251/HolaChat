#include "udp_server.hpp"
#include <vector>
#include <map>
#include <cstdlib>

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
}

void List(AddressList& addresslist, std::string& resp)
{
	auto bit = addresslist.book.begin();
	while (bit != addresslist.book.end())
	{
        resp += bit->first;
        resp += " ";
        bit++;
	}
}

int main(void)
{
	AddressList addresslist;

	UdpServer server;

	server.start("0", 9090, [&addresslist](const std::string& req, std::string& resp, const std::string& ip, uint16_t port,
        std::string& send_ip, uint16_t* send_port, int* is_send, std::string& msg_)
	{
		// 先判断是否是程序刚启动
		int pos = req.find(' ');
		std::string msg = req.substr(0, pos);

        int pos1 = req.find(' ', pos + 1);
		std::string name = req.substr(pos + 1, pos1 - pos - 1);

		// 如果是程序刚启动，就先记录该用户信息
		if (msg == "__client__start")
		{
			AddUser(name, ip, port, addresslist);
		}
		else if (msg == "__client__quit")
		{
			DeleteUser(name, addresslist);
		}
		else if (msg == "__client__list")
		{
			List(addresslist, resp);
		}
        else if (msg == "__client__message")
        {
            // 截取用户要给给发送的用户名

            std::string message = req.substr(pos1 + 1);

            auto ans = addresslist.book.find(name);

            if (ans != addresslist.book.end())
            {
                // 将对方的 ip 和 端口号拿出来
                send_ip = ans->second.first;
                *send_port = ans->second.second;
                *is_send = 1;

                msg_ = message;

                resp = "Send success!";
            }
            else
            {
                resp = "Current user does not exist!";
            }
        }
	}
	);
	return 0;
}
