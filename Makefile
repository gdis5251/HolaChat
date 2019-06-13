.PHONY:all
all:Chat_Client Chat_Server

Chat_Client:Chat_Client.cpp
	g++ -o $@ $^ -std=c++11 -g

Chat_Server:Chat_Server.cpp
	g++ -o $@ $^ -std=c++11 -g

.PHONY:clean
clean:
	rm Chat_Client Chat_Server
