.PHONY:all
all:Chat_Client Chat_Server

Chat_Client:Chat_Client.cpp udp_client.hpp udp_socket_api.hpp
	g++ -o $@ Chat_Client.cpp -std=c++11 -g -lpthread

Chat_Server:Chat_Server.cpp udp_server.hpp udp_socket_api.hpp
	g++ -o $@ Chat_Server.cpp -std=c++11 -g

.PHONY:clean
clean:
	rm Chat_Client Chat_Server
