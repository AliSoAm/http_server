#include "http_server.h"
#include "http_request.h"

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
    try
    {
      TCPRemoteClient client = tcpServer.Accept();
      HandleClient(client);
      client.Close();
    }
    catch (...)
    {
      cerr << "Unexpected exception !!!!! " << endl;
    }
  }
}

void HTTPServer::DispatchRequest(shared_ptr<HTTPRequest> request, TCPRemoteClient& client)
{
  try
  {
    cout << "dispatching->URI: "<< request->URI() << endl;
    string URI = request->URI();
    string name = "";
    size_t slashPos;
    subURI* current = &root;
    do
    {
      slashPos = URI.find("/", 1);
      size_t endNamePos;
      if (slashPos != string::npos)
        endNamePos = slashPos - 1;
      else
        endNamePos = string::npos;
      name = URI.substr(1, endNamePos);
      cout << "\tname: \"" << name << "\"" << endl;
      cout << "\tremaining URI: \"" << URI << "\"" << endl;
      auto next = find_if(current->children.begin(), current->children.end(), [&](shared_ptr<subURI> s){return s->name == name;});
      if (next != current->children.end())
        current = next->get();
      else
        break;
      URI.erase(0, slashPos);
    }while (slashPos != string::npos);
    if (URI == "" || URI == "/")
    {
      if (current->callback != NULL)
        current->callback(request);
    }
    else
    {
      throw HTTPException(HTTP_NOT_FOUND);
    }
    if (!request->isHeaderSent())
      HTTPServerErrorResponse(HTTP_NOT_IMPLEMENTED, client);
  }
  catch (HTTPException& e)
  {
    cout << "HTTP exception-> Code: " << e.HTTPErrorCode() << " ->>>> " << e.what() << endl;
    if (!request->isHeaderSent())
      HTTPServerErrorResponse(e.HTTPErrorCode(), client);
  }
  catch (exception &e)
  {
    cout << "STD exception->>>>" << e.what() << endl;
    if (!request->isHeaderSent())
      HTTPServerErrorResponse(500, client);
  }
}

void HTTPServer::HandleClient(TCPRemoteClient& client)
{
  try
  {
    auto request = make_shared<HTTPRequest>(client);
    DispatchRequest(request, client);
    request->Close();
  }
  catch (HTTPException& e)
  {
    cout << "HTTP exception-> Code: " << e.HTTPErrorCode() << " ->>>> " << e.what() << endl;
    HTTPServerErrorResponse(e.HTTPErrorCode(), client);
  }
  catch (TCPServerException &e)
  {
    cout << "TCPServer Exception->>>>" << e.what() << endl;
  }
  catch (exception &e)
  {
    cout << "Un handled exception ->>>>" << e.what() << endl;
    HTTPServerErrorResponse(500, client);
  }
}

void HTTPServer::HTTPServerErrorResponse(unsigned int errorCode, TCPRemoteClient& client)
{
  if (client.isOpen())
  {
    stringstream response;
    response << "HTTP/1.1 " << errorCode << " " << getHTTPResponseMessage(errorCode) << "\r\n"
             << "Content-Type: text/plain\r\n"
             << "Connection: close\r\n"
             << "Content-Length: 0\r\n"
             << "\r\n";
    client.Send(response.str().c_str(), response.str().length());
  }
}

shared_ptr<subURI> HTTPServer::addRootURI(const string& name, URICallback callback)
{
  root.children.emplace_back(make_shared<subURI>(name, callback));
  return root.children.back();
}

shared_ptr<subURI> HTTPServer::addURI(shared_ptr<subURI> parent, const string& name, URICallback callback)
{
  parent->children.emplace_back(make_shared<subURI>(name, callback));
  return parent->children.back();
}

void HTTPServer::setRootCallback(URICallback callback)
{
  root.callback = callback;
}
