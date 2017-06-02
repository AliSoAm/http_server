#include "TCPServer.h"
#include <stdexcept>
#include <cstring>
/*extern "C"
{
    #include "lwip/sockets.h"
}*/
#include <resolv.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

TCPRemoteClient::TCPRemoteClient(int socket): socketFD(socket)
{

}

int TCPRemoteClient::Send(const char* buffer, size_t length)
{
    return send(socketFD, buffer, length, 0);
}

int TCPRemoteClient::Recv(char* buffer, size_t length)
{
    return recv(socketFD, buffer, length, 0);
}

void TCPRemoteClient::Close()
{
    close(socketFD);
}

TCPServer::TCPServer(uint16_t port): port_(port)
{
        struct sockaddr_in serverAddress;
    if ((socketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw runtime_error("socket failed");
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
        throw runtime_error("bind failed");
    if (listen(socketFD, 5) != 0)
        throw runtime_error("listen failed");
}

TCPRemoteClient TCPServer::Accept()
{
    int clientFD;
    struct sockaddr_in client_addr;
    unsigned int addrlen = sizeof(client_addr);
    clientFD = accept(socketFD, (struct sockaddr*)&client_addr, &addrlen);
    return TCPRemoteClient(clientFD);
}
