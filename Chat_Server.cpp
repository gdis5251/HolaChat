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

	// 测试多个用户是否都被存进通讯录中
	auto bit = addresslist.book.begin();
	while (bit != addresslist.book.end())
	{
		std::cout << bit->first << std::endl;
		bit++;
	}
}

void DeleteUser(const std::string& name, AddressList& addresslist)
{
    addresslist.book.erase(addresslist.book.find(name));
}

void List(AddressList& addresslist, std::string& resp)
{
	// 测试多个用户是否都被存进通讯录中
	auto bit = addresslist.book.begin();
	while (bit != addresslist.book.end())
	{
		resp += bit->first;
		resp += '  ';
		bit++;
	}
}

int main(void)
{
	AddressList addresslist;

	UdpServer server;

	server.start("0", 9090, [&addresslist](const std::string & req, std::string & resp,
		const std::string & ip, uint16_t port)
	{
		// 先判断是否是程序刚启动
		int pos = req.find(' ');
		std::string msg = req.substr(0, pos);

		std::string name = req.substr(pos + 1);

		// 如果是程序刚启动，就先记录该用户信息
		if (msg == "__client__start")
		{
			std::cout << "Add User" << std::endl;
			AddUser(name, ip, port, addresslist);
		}
		else if (msg == "__client__quit")
		{
			std::cout << "Delete User" << std::endl;
			DeleteUser(name, addresslist);
		}
		else if (msg == "__client__list")
		{
			std::cout << "List All User" << std::endl;

			List(addresslist, resp);
		}

	}
	);
	return 0;
}
