#include "HTTPServer.h"
#include "HTTPRequest.h"

#include <stdexcept>
#include <cstring>
#include <cctype>
#include <resolv.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

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
HTTPServer::HTTPServer(unsigned int port)
{
    struct sockaddr_in serverAddress;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw runtime_error("socket failed");
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
        throw runtime_error("bind failed");
    if (listen(sockfd, 1) != 0)
        throw runtime_error("listen failed");
}

void HTTPServer::loop()
{
    while (1)
    {
        int clientfd;
        struct sockaddr_in client_addr;
        unsigned int addrlen = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
        cout << "Connection from: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
        HandleClient(clientfd, &client_addr, &addrlen);
    }
}

void HTTPServer::HandleClient(int clientfd, struct sockaddr_in* clientAddress, unsigned int *addressLen)
{
    HTTPRequest request = ParseRequest(clientfd);
    printf("\tRequest: method: %d, address: \"%s\", contentLenght:%lu\r\n",
        request.method(), request.URI().c_str(), request.contentLength());
    //DispatchRequest(request);
    close(clientfd);
}

HTTPRequest HTTPServer::ParseRequest(int clientfd)
{
    char buffer[100];
    string packet, header, payload;
    size_t headerEnd;
    do
    {
        int recvLen = recv(clientfd, buffer, 99, 0);
        buffer[recvLen] = '\0';
        packet += buffer;
        headerEnd = packet.find("\r\n\r\n");
    } while (headerEnd == string::npos);
    header = packet.substr(0, headerEnd);
    HTTPRequest request = ParseHeader(header);
    payload = packet.erase(0, headerEnd + 4);
    parsePayload(clientfd, request, payload);

    return request;
}

HTTPRequest HTTPServer::ParseHeader(const string& header)
{
    stringstream headerStream(header), lineStream;
    string line, methodStr, URI;
    getline(headerStream, line);
    lineStream.str(line);
    lineStream >> methodStr;
    HTTPMethod method = parseMethod(methodStr);
    lineStream >> URI;
    string host;
    MIMEType contentType = MIME_TYPE_NOT_SPECIFIED;
    std::vector<MIMEType> accept;
    HTTPTransferEncoding transferEncoding = HTTP_TRANSFER_ENCODING_NOT_SPECIFIED;
    HTTPConnection connection = HTTP_CONNECTION_NOT_SPECIFIED;
    size_t contentLength = 0;
    while (getline(headerStream, line))
    {
        size_t colonPosition = line.find(":");
        string name = line.substr(0, colonPosition);
        transform(name.begin(), name.end(), name.begin(), ::tolower);
        name.erase(remove_if(name.begin(), name.end(), ::isspace), name.end());
        string value = line.substr(colonPosition + 1);
        transform(value.begin(), value.end(), value.begin(), ::tolower);
        value.erase(remove_if(value.begin(), value.end(), ::isspace), value.end());
        if (colonPosition == string::npos)
            throw HTTPException(HTTP_BAD_REQUEST);
        else if (name == "host")
            host = value;
        else if (name == "content-type")
            contentType = parseContentType(value);
        else if (name == "accept")
            accept = parseAccept(value);
        else if (name == "connection")
            connection = parseConnection(value);
        else if (name == "transfer-encoding")
            transferEncoding = parseTransferEncoding(value);
        else if (name == "content-length")
            contentLength = stoi(value);
    }
    return HTTPRequest(method, URI, host, contentType, accept, connection, transferEncoding, contentLength);
}

HTTPMethod HTTPServer::parseMethod(std::string method)
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

MIMEType HTTPServer::parseContentType(std::string contentType)
{
    for (int type = MIME_APPLICATION_JAVASCRIPT; type <= MIME_IMAGE_GIF; type++)
        if (contentType == MIMEString[type])
            return (MIMEType)type;
    throw HTTPException(HTTP_BAD_REQUEST, "MIME type not found");
}

HTTPConnection HTTPServer::parseConnection(std::string connection)
{
    if (connection == CONNECTION_CLOSE)
        return HTTP_CONNECTION_CLOSE;
    if (connection == CONNECTION_KEEP_ALIVE)
        return HTTP_CONNECTION_KEEP_ALIVE;
    if (connection == CONNECTION_UPGRADE)
        return HTTP_CONNECTION_UPGRADE;
    throw HTTPException(HTTP_BAD_REQUEST, "Bad connection type");
}

HTTPTransferEncoding HTTPServer::parseTransferEncoding(std::string transferEncoding)
{
    if (transferEncoding == TRANSFER_ENCODING_CHUNKED)
        return HTTP_TRANSFER_ENCODING_CHUNKED;
    if (transferEncoding == TRANSFER_ENCODING_IDENTITY)
        return HTTP_TRANSFER_ENCODING_IDENTITY;
    throw HTTPException(HTTP_BAD_REQUEST, "Bad connection type");
}

vector<MIMEType> HTTPServer::parseAccept(std::string accept)
{
    vector<MIMEType> toBeReturned;
    for (int type = MIME_ALL; type <= MIME_IMAGE_GIF; type++)
        if (accept.find(MIMEString[type]) != string::npos)
            toBeReturned.push_back((MIMEType)type);
    return toBeReturned;
}

void HTTPServer::parsePayload(int clientfd, HTTPRequest& request, std::string remainingPayload)
{//XXX check for size
    if (request.transferEncoding() == HTTP_TRANSFER_ENCODING_CHUNKED)
    {
        stringstream chunkedPayload(remainingPayload);
        while (chunkedPayload.str().length() < 1000)
        {
            while (chunkedPayload.str().find("\r\n") != string::npos)
            {
                char buffer[100];
                int recvLen = recv(clientfd, buffer, 99, 0);
                if (recvLen <= 0)
                    throw HTTPException(HTTP_BAD_REQUEST);
                chunkedPayload.str(chunkedPayload.str() + buffer);
            }
            string chunkLenStr;
            getline(chunkedPayload, chunkLenStr);
            size_t chunkLen = stol(chunkLenStr, NULL, 16);
            if (chunkLen  == 0)
                return;
            while (chunkedPayload.str().size() < chunkLen + 2)
            {
                char buffer[100];
                int recvLen = recv(clientfd, buffer, 99, 0);
                if (recvLen <= 0)
                    throw HTTPException(HTTP_BAD_REQUEST);
                chunkedPayload.str(chunkedPayload.str() + buffer);
            }
            string chunk;
            for (size_t i = 0; i < chunkLen; i++)
                chunk += chunkedPayload.get();
            chunkedPayload.get();
            chunkedPayload.get();
            request.concatToPayload(chunk);
        }
    }
    else
    {
        request.concatToPayload(remainingPayload);
        while (request.payload().length() < request.contentLength() && request.payload().length() < 1000)
        {
            char buffer[100];
            int recvLen = recv(clientfd, buffer, 99, 0);
            if (recvLen <= 0)
                throw HTTPException(HTTP_BAD_REQUEST);
            buffer[recvLen] = '\0';
            request.concatToPayload(buffer);
        }
    }
}
