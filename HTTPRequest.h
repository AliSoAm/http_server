#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <string>
#include <cstdlib>
#include "HTTP.h"
class HTTPRequest
{
public:
    HTTPRequest(HTTPMethod method, std::string URI, std::string host, MIMEType contentType, std::vector<MIMEType> accept, HTTPConnection connection, HTTPTransferEncoding transferEncoding, size_t contentLength);
    HTTPMethod method() const;
    std::string URI() const;
    MIMEType contentType() const;
    std::vector<MIMEType> accept() const;
    std::string host() const;
    HTTPConnection connection() const;
    size_t contentLength() const;
    const std::string& payload() const;
    void concatToPayload(std::string payload);
private:
    HTTPMethod method_;
    std::string URI_;
    MIMEType contentType_;
    std::vector<MIMEType> accept_;
    std::string host_;
    HTTPConnection connection_;
    HTTPTransferEncoding transferEncoding_;
    size_t contentLength_;
    std::string payload_;
};

#endif
