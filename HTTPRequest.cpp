#include "HTTPRequest.h"

using namespace std;

HTTPRequest::HTTPRequest(HTTPMethod method, string URI, string host, MIMEType contentType, vector<MIMEType> accept, HTTPConnection connection, HTTPTransferEncoding transferEncoding, size_t contentLength)
                :method_(method), URI_(URI), contentType_(contentType), accept_(accept), connection_(connection), transferEncoding_(transferEncoding), contentLength_(contentLength)
{
    if (contentLength_ != 0 && transferEncoding == HTTP_TRANSFER_ENCODING_CHUNKED)
        throw HTTPException(HTTP_BAD_REQUEST, "TE with CL is not allowed");
}

HTTPMethod HTTPRequest::method() const
{
    return method_;
}

string HTTPRequest::URI() const
{
    return URI_;
}

MIMEType HTTPRequest::contentType() const
{
    return contentType_;
}

vector<MIMEType> HTTPRequest::accept() const
{
    return accept_;
}

string HTTPRequest::host() const
{
    return host_;
}

HTTPConnection HTTPRequest::connection() const
{
    return connection_;
}

size_t HTTPRequest::contentLength() const
{
    return contentLength_;
}

void HTTPRequest::concatToPayload(std::string payload)
{
    payload_ += payload;
}
