#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "HTTP.h"
#include "TCPServer.h"
#include "HTTPRequest.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

typedef std::function<void(std::shared_ptr<HTTPRequest>)> URICallback;
struct subURI
{
    std::vector<std::shared_ptr<subURI>> children;
    std::string name;
    URICallback callback;
    subURI(): callback(nullptr) {}
    subURI(const std::string& name, URICallback callback): name(name), callback(callback) {}
};

class HTTPServer
{
public:
                            HTTPServer                          (std::uint16_t port);
    void                    loop                                ();
    void                    setRootCallback                     (URICallback callback);
    std::shared_ptr<subURI> addRootURI                          (const std::string& name,
                                                                 URICallback callback);
    std::shared_ptr<subURI> addURI                              (std::shared_ptr<subURI> parent,
                                                                 const std::string& name,
                                                                 URICallback callback);
private:
    TCPServer               tcpServer;
    subURI                  root;
    void                    HandleClient                        (TCPRemoteClient& client);
    void                    DispatchRequest                     (std::shared_ptr<HTTPRequest> request,
                                                                 TCPRemoteClient& client);
    void                    HTTPServerErrorResponse             (unsigned int errorCode,
                                                                 TCPRemoteClient& client);
};

#endif
