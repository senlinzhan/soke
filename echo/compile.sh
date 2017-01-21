clang++ -std=c++11 -c ../TCPServer.cpp
clang++ -std=c++11 -c ../IPAddress.cpp
clang++ -std=c++11 -c ../TCPConnection.cpp
clang++ -std=c++11 -Wall -g  -o main EchoServerFork.cpp TCPServer.o IPAddress.o TCPConnection.o
