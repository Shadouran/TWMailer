#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <signal.h>

#include <initializer_list>

enum class SocketOption
{
    ReuseAddress,
    ReusePort
};

class Socket
{
public:
    Socket();

    void Create(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    void Start();

private:
    bool Accept(int& communicationSocket);
    void Bind();
    void SendData(int& communicationSocket, const std::string& data);
    void StartListening();
    void ClientCommunication(int& communicationSocket);
    void SetOptions(std::initializer_list<SocketOption> options);
    std::string GetLocalAddress(int socketInUse);

private:
    static constexpr int PORT = 6543;
    static constexpr int MAX_WAITING_CONNECTIONS = 5;
    static constexpr int BUFFER = 1024;

    int mSocket;
    socklen_t mAddressLength;
    struct sockaddr_in mAddress;
    struct sockaddr_in mClientAddress;
    bool mAbortRequested;
};