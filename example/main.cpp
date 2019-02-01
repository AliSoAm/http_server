#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "json.hpp"
#include "http_server.h"
#include "base64.h"

#include "file.hpp"

using namespace std;
using json = nlohmann::json;

bool MCBs[8] = {true, false, true, false, true, true, true, false};
bool Conductors[8];

void athuentication(shared_ptr<HTTPRequest> request)
{
  string ath;
  try
  {
    ath = request->headerParams("authorization");
  }
  catch (...)
  {
    throw HTTPException(HTTP_UNAUTHORIZED);
  }
  if (ath.find("Basic ") == string::npos)
    throw HTTPException(HTTP_BAD_REQUEST);
  ath = ath.substr(6);
  auto a = base64Decode(ath);
  a.push_back('\0');
  string cred = (char*)a.data();
  if (cred.find(":") == string::npos)
    throw HTTPException(HTTP_BAD_REQUEST);
  string username = cred.substr(0, cred.find(":"));
  string password = cred.substr(cred.find(":") + 1);
  if (username != "admin" || password != "1234")
    throw HTTPException(HTTP_FORBIDDEN);
}

static void MCBsGet(shared_ptr<HTTPRequest> request)
{
  json j;
  for (auto i = 1; i <= 8; i++)
  {
    json row;
    row["id"] = i;
    bool state = false;
    state = MCBs[i];
    if (state == true)
      row["state"] = "ON";
    else
      row["state"] = "OFF";
    row["current"] = to_string(i) + "A";
    j += row;
  }
  request->sendResponseHeader(HTTP_OK, MIME_APPICATION_JSON);
  string payload = j.dump();
  request->Send(payload.c_str(), payload.length());
}

static void conductorsGet(shared_ptr<HTTPRequest> request)
{
  json j;
  for (auto i = 1; i <= 8; i++)
  {
    json row;
    row["id"] = i;
    bool state = false;
    state = Conductors[i];
    if (state == true)
      row["state"] = "ON";
    else
      row["state"] = "OFF";
    j += row;
  }
  request->sendResponseHeader(HTTP_OK, MIME_APPICATION_JSON);
  string payload = j.dump();
  request->Send(payload.c_str(), payload.length());
}


static void conductorState(unsigned int id, string state)
{
  if (id >= 1 && id <= 8)
  {
    if (state == "ON")
    Conductors[id] = true;
    else if (state == "OFF")
    Conductors[id] = false;
    else
    throw HTTPException(HTTP_BAD_REQUEST);
    cout << "Conductors put-> id: " << id << " state: " << state << endl;
  }
  else
  {
    HTTPException(HTTP_NOT_FOUND);
  }
}

static void conductorsPut(shared_ptr<HTTPRequest> request)
{
  if (request->contentType() != MIME_APPICATION_JSON)
    throw HTTPException(HTTP_BAD_REQUEST);
  string payload;
  while (!request->isRecvCompleted())
  {
    char buff[50];
    int recvLen = request->Recv(buff, 50);
    buff[recvLen] = 0;
    payload += buff;
  }
  auto j = json::parse(payload.c_str());
  if (j.is_array())
  {
    for (auto i = j.begin(); i != j.end(); i++)
      conductorState(i->at("id").get<int>(), i->at("state").get<string>());
    request->sendResponseHeader(HTTP_OK, MIME_TEXT_HTML);
  }
  else
  {
    conductorState(j.at("id").get<int>(), j.at("state").get<string>());
    request->sendResponseHeader(HTTP_OK, MIME_TEXT_HTML);
  };
}

static void MCBCallback(shared_ptr<HTTPRequest> request)
{
  athuentication(request);
  if (request->method() == HTTP_METHOD_GET)
  {
    MCBsGet(request);
  }
}

static void conductorsCallback(shared_ptr<HTTPRequest> request)
{
  athuentication(request);
  if (request->method() == HTTP_METHOD_GET)
  {
    conductorsGet(request);
  }
  else if (request->method() == HTTP_METHOD_PUT)
  {
    conductorsPut(request);
  }
}

int main(int argc, char** argv)
{
  using namespace std::placeholders;
  if (argc < 2)
    return EXIT_FAILURE;
  HTTPServer server(atoi(argv[1]));
  auto indexCB = bind(fileCB, _1, "server/index.html", MIME_TEXT_HTML, "");
  auto styleCSSCB = bind(fileCB, _1, "server/style.css", MIME_TEXT_CSS, "");
  auto scriptJSCB = bind(fileCB, _1, "server/script.js", MIME_APPLICATION_JAVASCRIPT, "");
  auto jqueryJSCB = bind(fileCB, _1, "server/jquery.min.js.gz", MIME_TEXT_JAVASCRIPT, "gzip");
  auto bootstrapCSSCB = bind(fileCB, _1, "server/bootstrap.min.css.gz", MIME_TEXT_CSS, "gzip");
  auto bootstrapJSSCB = bind(fileCB, _1, "server/bootstrap.min.js.gz", MIME_TEXT_CSS, "gzip");
  server.setRootCallback(indexCB);
  server.addRootURI("style.css", styleCSSCB);
  server.addRootURI("script.js", scriptJSCB);
  server.addRootURI("jquery.min.js", jqueryJSCB);
  server.addRootURI("bootstrap.min.css", bootstrapCSSCB);
  server.addRootURI("bootstrap.min.js", bootstrapJSSCB);

  server.addRootURI("MCBs", MCBCallback);
  server.addRootURI("Conductors", conductorsCallback);
  try
  {
    server.loop();
  }
  catch (HTTPException& e)
  {
    cout << "Exception:" << e.what();
  }
  return EXIT_SUCCESS;
}
