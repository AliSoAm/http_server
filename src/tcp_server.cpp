#include "tcp_server.h"
#include <stdexcept>
#include <cstring>
/*extern "C"
{
  #include "lwip/sockets.h"
}*/
#include <resolv.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

using namespace std;

TCPServerException::TCPServerException(const char* what)
{
  strncpy(what_, what, 49);
  what_[49] = '\0';
}

const char* TCPServerException::what() const noexcept
{
  return what_;
}

TCPRemoteClient::TCPRemoteClient(int socket): socketFD(socket), closed(false)
{
  if (socketFD < 0)
    throw TCPServerException("Bad socket");
}

size_t TCPRemoteClient::Send(const char* buffer, size_t length)
{
  if (!isOpen())
    throw TCPServerException("Sending to closed connection");
  int sentLen = send(socketFD, buffer, length, MSG_NOSIGNAL);
  if (sentLen < 0)
  {
    Close();
    throw TCPServerException("Send faild");
  }
  return sentLen;
}

size_t TCPRemoteClient::Recv(char* buffer, size_t length)
{
  if (!isOpen())
    throw TCPServerException("Receiving from closed connection");
  size_t recvedLen = 0;
  fd_set read_fd_set;
  FD_ZERO(&read_fd_set);
  FD_SET(socketFD, &read_fd_set);
  struct timeval timeout;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  int selectReturn = select(socketFD + 1, &read_fd_set, NULL, NULL, &timeout);

  if (selectReturn > 0)
  {
    if (FD_ISSET(socketFD, &read_fd_set))
    {
      int recvLen =  recv(socketFD, buffer, length, 0);
      if (recvLen < 0)
      {
        Close();
        cerr << "Recv faild" << endl;
        throw TCPServerException("Recv faild");
      }
      if (recvLen == 0)
        closed = true;
      recvedLen = recvLen;
    }
    else
    {
      throw TCPServerException("Select error");
    }
  }
  else if (selectReturn < 0)
  {
    Close();
    throw TCPServerException("Select failed");
  }
  else
  {
  }
  return recvedLen;
}

bool TCPRemoteClient::isOpen() const
{
  return !closed;
}

void TCPRemoteClient::Close()
{
  if (isOpen())
  {
    closed = true;
    close(socketFD);
  }
}

TCPServer::TCPServer(uint16_t port): port_(port)
{
  struct sockaddr_in serverAddress;
  if ((socketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw TCPServerException("socket failed");
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  if (bind(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    throw TCPServerException("bind failed");
  if (listen(socketFD, 5) != 0)
    throw TCPServerException("listen failed");
}

TCPRemoteClient TCPServer::Accept()
{
  int clientFD;
  struct sockaddr_in client_addr;
  unsigned int addrlen = sizeof(client_addr);
  clientFD = accept(socketFD, (struct sockaddr*)&client_addr, &addrlen);
  if (clientFD < 0)
    throw TCPServerException("Accept failed");
  return TCPRemoteClient(clientFD);
}
