#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include "HTTP.h"
#include "TCPServer.h"
class HTTPRequest
{
public:
                                        HTTPRequest                     (TCPRemoteClient client);
    HTTPMethod                          method                          ()                                                  const;
    std::string                         URI                             ()                                                  const;
    MIMEType                            contentType                     ()                                                  const;
    std::vector<MIMEType>               accept                          ()                                                  const;
    std::string                         host                            ()                                                  const;
    HTTPConnection                      connection                      ()                                                  const;
    HTTPTransferEncoding                transferEncoding                ()                                                  const;
    void                                sendResponseHeader              (unsigned int responseCode,
                                                                         MIMEType contentType);
    void                                Send                            (const char* buffer,
                                                                         size_t length);
    size_t                              Recv                            (char* buffer,
                                                                         size_t length);
    bool                                isHeaderSent                    ()                                                  const;
    bool                                isRecvCompleted                 ()                                                  const;
    void                                Close                           ();
    std::string                         params                          (const std::string& name);
private:
    TCPRemoteClient                     client_;
    HTTPMethod                          method_;
    std::string                         URI_;
    MIMEType                            contentType_;
    std::vector<MIMEType>               accept_;
    std::string                         host_;
    HTTPConnection                      connection_;
    HTTPTransferEncoding                transferEncoding_;
    size_t                              contentLength_;
    std::vector<char>                   payloadRemaining;
    char                                remainingBuffer[150];
    size_t                              remainingBufferLen;
    size_t                              remainingChunkLen;
    size_t                              contentReceived;
    bool                                recvComplete;
    bool                                headerSent;
    std::map<std::string, std::string>  params_;
    void                                ParseRequest                        ();
    void                                ParseHeader                         (const std::string& header);
    HTTPMethod                          parseMethod                         (const std::string& method)                     const;
    MIMEType                            parseContentType                    (const std::string& contentType)                const;
    HTTPConnection                      parseConnection                     (const std::string& connection)                 const;
    HTTPTransferEncoding                parseTransferEncoding               (const std::string& transferEncoding)           const;
    std::vector<MIMEType>               parseAccept                         (const std::string& accept)                     const;
    void                                parseParams                         ();
    void                                parseParams                         (const std::string& paramsStr);
    void                                parseURIParams                      ();
    void                                parsePayloadParams                  ();
    void                                prepareForNextChunk                 ();
    size_t                              recvRemainingChunk                  (char* buffer,
                                                                             size_t length,
                                                                             size_t recvedLen);
    size_t                              recvRemainingBuffer                 (char* buffer,
                                                                             size_t length,
                                                                             size_t recvedLen);
    void                                completeCurrentChunk                ();
};

#endif
