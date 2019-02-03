#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <map>
#include <vector>
#include <string>
#include <cstdlib>

#include <nlohmann/json.hpp>
#include "json.hpp"
#include "http.h"
#include "tcp_server.h"
class HTTPRequest
{
public:
                                      HTTPRequest                     (TCPRemoteClient client);
  void                                parseUrlPatterns                (const std::string& pattern);
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
  nlohmann::json                      params                          (const std::string& name);
  std::string                         headerParams                    (const std::string& name);
  void                                addResponseHeaderParameter      (const std::string& name,
                                                                       const std::string& value);
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
  std::string                         responseHeaderParams;
  std::map<std::string, std::string>  headerParams_;
  nlohmann::json                      params_;
  void                                ParseRequest                        ();
  void                                ParseHeader                         (const std::string& header);
  HTTPMethod                          parseMethod                         (const std::string& method)                     const;
  MIMEType                            parseContentType                    (std::string contentType)                       const;
  HTTPConnection                      parseConnection                     (std::string connection)                        const;
  HTTPTransferEncoding                parseTransferEncoding               (std::string transferEncoding)                  const;
  std::vector<MIMEType>               parseAccept                         (std::string accept)                            const;
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
