FLAG=tcp_socket.hpp tcp_epoll_server.hpp tcp_client.hpp

.PHONY:all
all: client server

client:chat_client.cpp $(FLAG)
	g++ -o $@ chat_client.cpp -std=c++11 -lpthread -g

server:chat_server.cpp $(FLAG)
	g++ -o $@ chat_server.cpp -std=c++11 -lpthread -g

.PHONY:clean
clean:
	rm client server
