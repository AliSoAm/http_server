#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>
#include "http.h"
#include "tcp_server.h"
#include "http_request.h"

typedef std::function<void(std::shared_ptr<HTTPRequest>)> PatternCallback;

class HTTPServer
{
public:
                          HTTPServer                          (std::uint16_t port);
  void                    loop                                ();
  void                    addPattern                          (const std::string& pattern,
                                                               PatternCallback callback);
private:
  TCPServer               tcpServer;
  void                    HandleClient                        (TCPRemoteClient& client);
  void                    DispatchRequest                     (std::shared_ptr<HTTPRequest> request,
                                                               TCPRemoteClient& client);
  void                    HTTPServerErrorResponse             (unsigned int errorCode,
                                                               TCPRemoteClient& client);
  std::vector<std::pair<std::string, PatternCallback>> dispatchPatterns;
};

#endif
