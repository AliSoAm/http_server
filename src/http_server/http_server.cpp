#include "http_server.hpp"

#include <regex>
#include <string>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

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

void HTTPServer::addRoute(const std::string& pattern, const PatternCallback& callback)
{
  dispatchPatterns.emplace_back(std::make_pair(pattern, callback));
}

void HTTPServer::DispatchRequest(shared_ptr<HTTPRequest> request, TCPRemoteClient& client)
{
  try
  {
    cout << "dispatching->URI: "<< request->URI() << endl;
    std::string url = request->URI();
    for (auto& pattern: dispatchPatterns)
    {
      std::string patternRegexString = std::regex_replace (pattern.first, std::regex("<int:[\\w]+>"), "\\d+");
      patternRegexString = std::regex_replace (patternRegexString, std::regex("<string:[\\w]+>"), "[\\w\\.]+");
      std::regex patternRegex(patternRegexString);
      if (std::regex_match(url, patternRegex))
      {
        request->parseUrlPatterns(pattern.first);
        pattern.second(request);
        return;
      }
    }
    std::cerr << "Not found" << std::endl;
    HTTPServerErrorResponse(HTTP_NOT_IMPLEMENTED, client);
  }
  catch (HTTPException& e)
  {
    cerr << "HTTP exception-> Code: " << e.HTTPErrorCode() << " ->>>> " << e.what() << endl;
    if (!request->isHeaderSent())
      HTTPServerErrorResponse(e.HTTPErrorCode(), client);
  }
  catch (exception &e)
  {
    cerr << "STD exception->>>>" << e.what() << endl;
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
