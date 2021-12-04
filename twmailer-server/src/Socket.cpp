#include "Socket.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

Socket::Socket()
    : mSocket(-1), mAddressLength(-1), mAbortRequested(false)
{
    mAddress = {};
    mClientAddress = {};
}

void Socket::Create(int domain, int type, int protocol)
{
    if ((mSocket = socket(domain, type, protocol)) == -1)
    {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }
}

void Socket::Bind()
{
    mAddress.sin_family = AF_INET;
    mAddress.sin_addr.s_addr = INADDR_ANY;
    mAddress.sin_port = htons(PORT);

    if (bind(mSocket, reinterpret_cast<struct sockaddr*>(&mAddress), sizeof(mAddress)) == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
}

void Socket::StartListening()
{
    if (listen(mSocket, MAX_WAITING_CONNECTIONS) == -1)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
}

bool Socket::Accept(int& communicationSocket)
{
    if ((communicationSocket = accept(mSocket, reinterpret_cast<struct sockaddr*>(&mClientAddress), &mAddressLength)) == -1)
    {
        mAbortRequested ? perror("accept error after arbort") : perror("accept error");
        return false;
    }
    return true;
}

void Socket::Start()
{
    Bind();
    SetOptions({ SocketOption::ReuseAddress, SocketOption::ReusePort });
    StartListening();

    while (!mAbortRequested)
    {
        int newSocket{ -1 };
        std::cout << "Waiting for connections...\n";
        mAddressLength = sizeof(struct sockaddr_in);
        if (!Accept(newSocket))
            break;

        std::cout << "Client connected from " << inet_ntoa(mClientAddress.sin_addr) << ":" << ntohs(mClientAddress.sin_port) << "\n";
        ClientCommunication(newSocket);
    }
}

void Socket::ClientCommunication(int& communicationSocket)
{
    int size;
    char buffer[BUFFER];
    strcpy(buffer, "Connected to the server!\r\nEnter your commands!\r\n");
    SendData(communicationSocket, buffer);

    do
    {
        size = recv(communicationSocket, buffer, BUFFER - 1, 0);
        if (size == -1)
        {
            if (mAbortRequested)
            {
                perror("recv error after aborted");
            }
            else
            {
                perror("recv error");
            }
            break;
        }

        if (size == 0)
        {
            printf("Client closed remote socket\n"); // ignore error
            break;
        }

        // remove ugly debug message, because of the sent newline of client
        if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n')
        {
            size -= 2;
        }
        else if (buffer[size - 1] == '\n')
        {
            --size;
        }

        buffer[size] = '\0';
        printf("Message received: %s\n", buffer); // ignore error

        SendData(communicationSocket, "OK");
    } while (strcmp(buffer, "quit") != 0 && !mAbortRequested);

    // closes/frees the descriptor if not already
    if (communicationSocket != -1)
    {
        if (shutdown(communicationSocket, SHUT_RDWR) == -1)
        {
            perror("shutdown new_socket");
        }
        if (close(communicationSocket) == -1)
        {
            perror("close new_socket");
        }
        communicationSocket = -1;
    }

    exit(EXIT_FAILURE);
}

void Socket::SendData(int& communicationSocket, const std::string& data)
{
    if (send(communicationSocket, data.c_str(), data.length(), 0) == -1)
    {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
}

void Socket::SetOptions(std::initializer_list<SocketOption> options)
{
    int reuseValue{ 1 };

    for (auto option : options)
        switch (option)
        {
        case SocketOption::ReuseAddress:
            if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &reuseValue, sizeof(reuseValue)) == -1)
            {
                perror("set socket options - reuseAddr");
                exit(EXIT_FAILURE);
            }
            break;
        case SocketOption::ReusePort:
            if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEPORT, &reuseValue, sizeof(reuseValue)) == -1)
            {
                perror("set socket options - reusePort");
                exit(EXIT_FAILURE);
            }
            break;
        }
}

std::string Socket::GetLocalAddress(int socketInUse)
{
    struct sockaddr localAddress {};
    socklen_t localAddressLength{};
    getsockname(socketInUse, &localAddress, &localAddressLength);
    return std::string(localAddress.sa_data);
}