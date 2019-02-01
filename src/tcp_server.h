#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <cstdint>
#include <string>
#include <exception>

typedef int Socket;
class TCPServerException: public std::exception
{
public:
    TCPServerException(const char* what);
    virtual const char* what() const noexcept;
private:
    char what_[50];
};

class TCPRemoteClient
{
public:
                            TCPRemoteClient                 (Socket socket);
    size_t                  Send                            (const char* buffer,
                                                             size_t length);
    size_t                  Recv                            (char* buffer,
                                                             size_t length);
    bool                    isOpen                          ()                                     const;
    void                    Close                           ();
private:
    Socket                  socketFD;
    bool                    closed;
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
