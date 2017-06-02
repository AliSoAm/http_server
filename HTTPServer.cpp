#include "HTTPServer.h"
#include "HTTPRequest.h"

#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

HTTPServer::HTTPServer(uint16_t port): tcpServer(port)
{
}

void HTTPServer::loop()
{
    while (1)
    {
        TCPRemoteClient client = tcpServer.Accept();
//        cout << "Connection from: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
        HandleClient(client);
        client.Close();
    }
}

void HTTPServer::DispatchRequest(HTTPRequest& request, TCPRemoteClient& client)
{
    try
    {
        cout << "dispatching->URI: "<< request.URI() << endl;
        string URI = request.URI();
        string name = "";
        size_t slashPos;
        subURI* current = &root;
        do
        {
            slashPos = URI.find("/", 1);
            name = URI.substr(1, slashPos);
            URI.erase(0, slashPos);
            cout << "\tname: \"" << name << "\"" << endl;
            cout << "\tremaining URI: \"" << URI << "\"" << endl;
            auto next = find_if(current->children.begin(), current->children.end(), [&](subURI* s){return s->name == name;});
            if (next != current->children.end())
                current = *next;
            else
                break;
        }while (slashPos != string::npos);
        if (URI == "")
        {
            if (current->callback != NULL)
                current->callback(request);
        }
        else
        {
            throw HTTPException(HTTP_NOT_FOUND);
        }
        if (!request.isHeaderSent())
            HTTPServerErrorResponse(HTTP_NOT_IMPLEMENTED, client);
    }
    catch (HTTPException& e)
    {
        cout << "HTTP exception-> Code: " << e.HTTPErrorCode() << " ->>>> " << e.what() << endl;
        if (!request.isHeaderSent())
            HTTPServerErrorResponse(e.HTTPErrorCode(), client);
    }
    catch (exception &e)
    {
        cout << "Un handled exception->>>>" << e.what() << endl;
        if (!request.isHeaderSent())
            HTTPServerErrorResponse(500, client);
    }
}

void HTTPServer::HandleClient(TCPRemoteClient& client)
{
    try
    {
        HTTPRequest request(client);
        DispatchRequest(request, client);
        request.Close();
    }
    catch (HTTPException& e)
    {
        cout << "HTTP exception-> Code: " << e.HTTPErrorCode() << " ->>>> " << e.what() << endl;
        HTTPServerErrorResponse(e.HTTPErrorCode(), client);
    }
    catch (exception &e)
    {
        cout << "Un handled exception->>>>" << e.what() << endl;
        HTTPServerErrorResponse(500, client);
    }

}

void HTTPServer::HTTPServerErrorResponse(unsigned int errorCode, TCPRemoteClient& client)
{
    stringstream response;
    response << "HTTP/1.1 " << errorCode << " " << getHTTPResponseMessage(errorCode) << "\r\n"
             << "Content-Type: text/plain\r\n"
             << "Connection: close\r\n"
             << "Content-Length: 0\r\n"
             << "\r\n";
    client.Send(response.str().c_str(), response.str().length());
}
subURI* HTTPServer::addRootURI(const string& name, URICallback callback)
{
    subURI* toBeReturned = new subURI;
    toBeReturned->name = name;
    toBeReturned->callback = callback;
    root.children.push_back(toBeReturned);
    return toBeReturned;
}

subURI* HTTPServer::addURI(subURI& parent, const string& name, URICallback callback)
{
    subURI* toBeReturned = new subURI;
    toBeReturned->name = name;
    toBeReturned->callback = callback;
    parent.children.push_back(toBeReturned);
    return toBeReturned;
}

void HTTPServer::setRootCallback(URICallback callback)
{
    root.callback = callback;
}
