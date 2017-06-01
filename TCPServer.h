#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <cstdint>
#include <string>

typedef int Socket;
class TCPRemoteClient
{
public:
                            TCPRemoteClient                 (Socket socket);
    int                     Send                            (const char* buffer,
                                                             size_t length);
    int                     Recv                            (char* buffer,
                                                             size_t length);
    void                    Close                           ();
private:
    Socket                  socketFD;
};

class TCPServer
{
public:
                            TCPServer                       (std::uint16_t port);
    std::uint16_t           port                            () const {return port_;}
    TCPRemoteClient         Accept                          ();
private:
    std::uint16_t           port_;
    Socket                  socketFD;
};

#endif
