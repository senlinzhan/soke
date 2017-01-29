clang++ -std=c++11 -c ../TCPServer.cpp
clang++ -std=c++11 -c ../IPAddress.cpp
clang++ -std=c++11 -c ../TCPConnection.cpp
clang++ -std=c++11 -c ../TCPClient.cpp
clang++ -std=c++11 -Wall -g  -o server EchoServerFork.cpp TCPServer.o IPAddress.o TCPConnection.o
clang++ -std=c++11 -Wall -g  -o client EchoClient.cpp TCPClient.o
