#include <iostream>
#include "Socket.hpp"

int main(int argc, char **argv)
{
    Socket socket;
    socket.Create();
    socket.Start();

    return 0;
}