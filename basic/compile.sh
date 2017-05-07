clang++ -std=c++11 -Wall -g -c ../TCPServer.cpp
clang++ -std=c++11 -Wall -g -c ../IPAddress.cpp
clang++ -std=c++11 -Wall -g -c ../TCPConnection.cpp
clang++ -std=c++11 -Wall -g -c ../TCPClient.cpp
clang++ -std=c++11 -Wall -g  -o server EchoServerPoll.cpp TCPServer.o IPAddress.o TCPConnection.o
clang++ -std=c++11 -Wall -g  -o client EchoClient.cpp TCPClient.o

