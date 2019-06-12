.PHONY:all
all:HelloChat_Client HelloChat_Server

HelloChat_Client:HelloChat_client.cpp
	g++ -o $@ $^ -std=c++11 -g

HelloChat_Server:HelloChat_server.cpp
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm HelloChat_Client HelloChat_Server
