#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "HTTP.h"
#include "TCPServer.h"
#include "HTTPRequest.h"
#include <cstdint>
#include <vector>

typedef void(*URICallback)(HTTPRequest& request);
struct subURI
{
    std::vector<subURI*> children;
    std::string name;
    URICallback callback;
    subURI(): callback(NULL) {}
};

class HTTPServer
{
public:
                            HTTPServer                          (std::uint16_t port);
    void                    loop                                ();
    void                    setRootCallback                     (URICallback callback);
    subURI*                 addRootURI                          (const std::string& name,
                                                                 URICallback callback);
    subURI*                 addURI                              (subURI& parent,
                                                                 const std::string& name,
                                                                 URICallback callback);
private:
    TCPServer               tcpServer;
    subURI                  root;
    void                    HandleClient                        (TCPRemoteClient& client);
    void                    DispatchRequest                     (HTTPRequest& request,
                                                                 TCPRemoteClient& client);
    void                    HTTPServerErrorResponse             (unsigned int errorCode,
                                                                 TCPRemoteClient& client);
};

#endif
