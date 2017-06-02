#include "HTTPRequest.h"
#include <stdexcept>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

const char MIMEString[][40] =
{
    "",
    "*/*",
    "application/javascript",
    "application/json",
    "application/x-www-form-urlencoded",
    "application/xml",
    "application/zip",
    "application/pdf",
    "audio/mpeg",
    "audio/vorbis",
    "multipart/form-data",
    "text/css",
    "text/html",
    "text/plain",
    "image/png",
    "image/jpeg",
    "image/gif",
};

HTTPRequest::HTTPRequest(TCPRemoteClient client): client_(client), contentLength_(0), remainingBufferLen (0), remainingChunkLen(0), contentReceived(0), recvComplete(false), headerSent(false)
{
    remainingBuffer[99] = 0;
    ParseRequest();
    if (contentLength_ != 0 && transferEncoding_ == HTTP_TRANSFER_ENCODING_CHUNKED)
        throw HTTPException(HTTP_BAD_REQUEST, "TE with CL is not allowed");
    if (transferEncoding_ == HTTP_TRANSFER_ENCODING_NOT_SPECIFIED && contentLength_ == 0)
        recvComplete = true;
}

void HTTPRequest::Close()
{
    if (1)
        client_.Send("0\r\n\r\n", 5);
}

void HTTPRequest::ParseRequest()
{
    char buffer[100];

    string packet, header, payload;
    size_t headerEnd = 0;
    size_t bufferEnd = 0;
    int recvLen = 0;
    do
    {
        recvLen = client_.Recv(buffer, 99);
        if (recvLen <= 0)
        {
            //XXX
        }
        buffer[recvLen] = '\0';
        bufferEnd += recvLen;
        packet += buffer;
        headerEnd = packet.find("\r\n\r\n");
    } while (headerEnd == string::npos);
    size_t payloadStart = recvLen - (bufferEnd - (headerEnd + 4));
    remainingBufferLen = bufferEnd - (headerEnd + 4);
    memcpy(remainingBuffer, buffer + payloadStart, remainingBufferLen);
    remainingBuffer[remainingBufferLen] = 0;
    header = packet.substr(0, headerEnd);
    ParseHeader(header);
    payload = packet.erase(0, headerEnd + 4);
}

void HTTPRequest::ParseHeader(const string& header)
{
    stringstream headerStream(header), lineStream;
    string line, methodStr;
    getline(headerStream, line);
    lineStream.str(line);
    lineStream >> methodStr;
    method_ = parseMethod(methodStr);
    lineStream >> URI_;
    contentType_ = MIME_TYPE_NOT_SPECIFIED;
    transferEncoding_ = HTTP_TRANSFER_ENCODING_NOT_SPECIFIED;
    connection_ = HTTP_CONNECTION_NOT_SPECIFIED;
    size_t contentLength = 0;
    while (getline(headerStream, line))
    {
        if (*line.rbegin() == '\r')
            line.erase(line.length() - 1);
        size_t colonPosition = line.find(":");
        string name = line.substr(0, colonPosition);
        transform(name.begin(), name.end(), name.begin(), [&](char ch) -> char {if (ch >= 'A' && ch <= 'Z') ch += 32; return ch;});
        name.erase(remove(name.begin(), name.end(), ' '), name.end());
        string value = line.substr(colonPosition + 1);
        transform(value.begin(), value.end(), value.begin(), [&](char ch) -> char {if (ch >= 'A' && ch <= 'Z') ch += 32; return ch;});
        value.erase(remove(value.begin(), value.end(), ' '), value.end());
        if (colonPosition == string::npos)
            throw HTTPException(HTTP_BAD_REQUEST);
        else if (name == "host")
            host_ = value;
        else if (name == "content-type")
            contentType_ = parseContentType(value);
        else if (name == "accept")
            accept_ = parseAccept(value);
        else if (name == "connection")
            connection_ = parseConnection(value);
        else if (name == "transfer-encoding")
            transferEncoding_ = parseTransferEncoding(value);
        else if (name == "content-length")
            contentLength_ = atoi(value.c_str());
    }
    parseParams();
}

void HTTPRequest::parseParams()
{
    parseURIParams();
    parsePayloadParams();
}

void HTTPRequest::parseURIParams()
{
    size_t paramsPos = URI_.find('?');
    if (paramsPos != string::npos)
    {
        string paramsStr = URI_.substr(paramsPos + 1);
        URI_.erase(paramsPos);
        parseParams(paramsStr);
    }
}

void HTTPRequest::parsePayloadParams()
{
    if (contentType_ == MIME_APPLICATION_X_WWW_FORM_URLENCODED)
    {
        string payload;
        while (!isRecvCompleted())
        {
            char buff[50];
            int recvLen = Recv(buff, 50);
            buff[recvLen] = 0;
            payload += buff;
        }
        parseParams(payload);
    }
}

void HTTPRequest::parseParams(const string& paramsStr)
{
    stringstream params(paramsStr);
    string param;
    while (getline(params, param, '&'))
    {
        size_t equaleSignPosition = param.find('=');
        if (equaleSignPosition == string::npos)
            continue;
        string name = param.substr(0, equaleSignPosition);
        string value = param.substr(equaleSignPosition + 1);
        params_[name] = value;
    }
}
HTTPMethod HTTPRequest::parseMethod(const string& method) const
{
    if (method == "GET")
        return HTTP_METHOD_GET;
    if (method == "POST")
        return HTTP_METHOD_POST;
    if (method == "PUT")
        return HTTP_METHOD_PUT;
    if (method == "DELETE")
        return HTTP_METHOD_DELETE;
    throw HTTPException(HTTP_NOT_IMPLEMENTED);
}

MIMEType HTTPRequest::parseContentType(const string& contentType) const
{
    cout << "Content-Type: \"" << contentType << "\""<< endl;
    for (int type = MIME_APPLICATION_JAVASCRIPT; type <= MIME_IMAGE_GIF; type++)
        if (contentType.find(MIMEString[type]) != string::npos)
            return (MIMEType)type;
    throw HTTPException(HTTP_BAD_REQUEST, "MIME type not found");
}

HTTPConnection HTTPRequest::parseConnection(const string& connection) const
{
    if (connection == CONNECTION_CLOSE)
        return HTTP_CONNECTION_CLOSE;
    if (connection == CONNECTION_KEEP_ALIVE)
        return HTTP_CONNECTION_KEEP_ALIVE;
    if (connection == CONNECTION_UPGRADE)
        return HTTP_CONNECTION_UPGRADE;
    throw HTTPException(HTTP_BAD_REQUEST, "Bad connection type");
}

HTTPTransferEncoding HTTPRequest::parseTransferEncoding(const string& transferEncoding) const
{
    cout << "Transfer-Encoding: \"" << transferEncoding << "\"" << endl;
    if (transferEncoding == TRANSFER_ENCODING_CHUNKED)
        return HTTP_TRANSFER_ENCODING_CHUNKED;
    if (transferEncoding == TRANSFER_ENCODING_IDENTITY)
        return HTTP_TRANSFER_ENCODING_IDENTITY;
    throw HTTPException(HTTP_BAD_REQUEST, "Bad Transfer-Encoding");
}

vector<MIMEType> HTTPRequest::parseAccept(const string& accept) const
{
    cout << "Accept: \"" << accept << "\"" << endl;
    vector<MIMEType> toBeReturned;
    for (int type = MIME_ALL; type <= MIME_IMAGE_GIF; type++)
        if (accept.find(MIMEString[type]) != string::npos)
            toBeReturned.push_back((MIMEType)type);
    return toBeReturned;
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

HTTPTransferEncoding HTTPRequest::transferEncoding() const
{
    return transferEncoding_;
}

void HTTPRequest::Send(const char* buffer, size_t length)
{
    char sendBuffer[100];
    size_t remainedLength = length;
    while (remainedLength > 0)
    {
        if (remainedLength > (100 - 7))
        {
            sprintf(sendBuffer, "%03x\r\n", 100 - 7);
            memcpy(sendBuffer + 5, buffer, 100 - 7);
            sendBuffer[100 - 2] = '\r';
            sendBuffer[100 - 1] = '\n';
            if (client_.Send(sendBuffer, 100) != 100)
                throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
            buffer += 100 - 7;
            remainedLength -= 100 - 7;
        }
        else
        {
            sprintf(sendBuffer, "%03x\r\n", remainedLength);
            memcpy(sendBuffer + 5, buffer, remainedLength);
            sendBuffer[remainedLength + 5] = '\r';
            sendBuffer[remainedLength + 6] = '\n';
            if (client_.Send(sendBuffer, remainedLength + 7) != remainedLength + 7)
                throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
            buffer += remainedLength;
            remainedLength = 0;
        }
    }
}

int HTTPRequest::Recv(char* buffer, size_t length)
{
    if (isRecvCompleted())
        return 0;
    size_t recvedLen = 0;
    if (transferEncoding_ == HTTP_TRANSFER_ENCODING_CHUNKED)
    {
        while (recvedLen < length && !isRecvCompleted())
        {
            if (remainingChunkLen > 0)
            {
                if (remainingBufferLen > 0)
                {
                    recvedLen = recvRemainingBuffer(buffer, length, recvedLen);
                }
                else
                {
                    recvedLen = recvRemainingChunk(buffer, length, recvedLen);
                    if (remainingChunkLen > 0 && length != recvedLen)
                        break;
                }
                if (remainingChunkLen == 0)
                    completeCurrentChunk();
            }
            else
            {
                prepareForNextChunk();
            }
        }
    }
    else
    {
        if (remainingBufferLen > 0)
        {
            memcpy(buffer, remainingBuffer, remainingBufferLen);
            recvedLen = remainingBufferLen;
            remainingBufferLen = 0;
            contentReceived += recvedLen;
        }
        if (length > recvedLen && contentReceived < contentLength_)
        {
            int recvLen = client_.Recv(buffer + recvedLen, length - recvedLen);
            if (recvLen < 0)
                throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
            contentReceived += recvLen;
            recvedLen += recvLen;
        }
        if (contentReceived >= contentLength_)
            recvComplete = true;
    }
    return recvedLen;
}

size_t HTTPRequest::recvRemainingBuffer(char* buffer, size_t length, size_t recvedLen)
{
    size_t readyLen = (remainingChunkLen > remainingBufferLen) ? remainingBufferLen : remainingChunkLen;
    if (length <= readyLen)
    {
        memcpy(buffer + recvedLen, remainingBuffer, length);
        recvedLen += length;
        remainingChunkLen -= length;
        remainingBufferLen -= length;
        memcpy(remainingBuffer, remainingBuffer + length, remainingBufferLen);
    }
    else
    {
        memcpy(buffer + recvedLen, remainingBuffer, readyLen);
        recvedLen += readyLen;
        remainingChunkLen -= readyLen;
        remainingBufferLen -= readyLen;
        memcpy(remainingBuffer, remainingBuffer + readyLen, remainingBufferLen);
    }
    remainingBuffer[remainingBufferLen] = 0;
    return recvedLen;
}

int HTTPRequest::recvRemainingChunk(char* buffer, size_t length, size_t recvedLen)
{
    int recvLen = 0;
    if (length < remainingChunkLen)
        recvLen = client_.Recv(buffer + recvedLen, (length - recvedLen));
    else
        recvLen = client_.Recv(buffer + recvedLen, remainingChunkLen);
    if (recvLen <= 0)//XXX <= ?
        throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
    remainingChunkLen -= recvLen;
    recvedLen += recvLen;
    return recvedLen;
}

void HTTPRequest::completeCurrentChunk()
{
    if (remainingBufferLen >= 2)
    {
        memcpy(remainingBuffer, remainingBuffer + 2, remainingBufferLen - 2);
        remainingBufferLen -= 2;
        remainingBuffer[remainingBufferLen] = 0;
    }
    else if (remainingBufferLen == 1)
    {
        char buff[1];
        remainingBufferLen = 0;
        remainingBuffer[0] = 0;
        if (client_.Recv(buff, 1) != 1)
            throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
    }
    else
    {
        char buff[2];
        if (client_.Recv(buff, 2) != 2)
            throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
    }
}

void HTTPRequest::prepareForNextChunk()
{
    int recvLen = remainingBufferLen;
    size_t startChunk = 0;
    string chunk(remainingBuffer);
    if (chunk.find("\r\n") == string::npos)
    {
        recvLen = client_.Recv(remainingBuffer, 99);
        if (recvLen <= 0)
            throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
        remainingBuffer[recvLen] = 0;
        chunk += remainingBuffer;
        startChunk = chunk.find("\r\n") + 2 - remainingBufferLen;
    }
    else
    {
        startChunk = chunk.find("\r\n") + 2;
    }
    if (chunk.find("\r\n") == string::npos)
        throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
    remainingChunkLen = strtol(chunk.c_str(), NULL, 16);
    if (remainingChunkLen == 0)
    {
        recvComplete = true;
        return;
    }
    remainingBufferLen = recvLen - startChunk;
    memcpy(remainingBuffer, remainingBuffer + startChunk, remainingBufferLen);
    remainingBuffer[remainingBufferLen] = 0;
    return;
}

bool HTTPRequest::isHeaderSent() const
{
    return headerSent;
}

bool HTTPRequest::isRecvCompleted() const
{
    return recvComplete;
}

void HTTPRequest::sendResponseHeader(unsigned int responseCode, MIMEType contentType)
{
    if (isHeaderSent())
        return;
    stringstream response;
    response << "HTTP/1.1 " << to_string(responseCode) << " " << getHTTPResponseMessage(responseCode) << "\r\n"
             << "Content-Type: " << MIMEString[contentType] << "\r\n"
             << "Connection: close\r\n"
             << "Transfer-Encoding:chunked\r\n"
             << "\r\n";
    client_.Send(response.str().c_str(), response.str().length());
    headerSent = true;
}

string HTTPRequest::params(const string& name)
{
    return params_[name];
}
