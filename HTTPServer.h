#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include "HTTP.h"
#include "HTTPRequest.h"
#include <vector>

class HTTPServer
{
public:
                            HTTPServer                          (unsigned int port);
    void                    loop                                ();
private:
    int sockfd;

    void                    HandleClient                        (int clientfd,
                                                                 struct sockaddr_in* clientAddress,
                                                                 unsigned int *addressLen);
    HTTPRequest             ParseRequest                        (int clientfd);
    HTTPRequest             ParseHeader                         (const std::string& header);
    HTTPMethod              parseMethod                         (std::string method);
    MIMEType                parseContentType                    (std::string contentType);
    HTTPConnection          parseConnection                     (std::string connection);
    HTTPTransferEncoding    parseTransferEncoding               (std::string transferEncoding);
    std::vector<MIMEType>   parseAccept                         (std::string accept);
    void                    parsePayload                        (int clientfd, HTTPRequest& request,
                                                                 std::string payload);
};

#endif
