#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <nlohmann/json.hpp>
#include <http_server/http_server.hpp>
#include <http_server/utils/base64.hpp>
#include <controllers/file_controller.hpp>

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

void testCallback(std::shared_ptr<HTTPRequest> request)
{
  cout << request->params("file_name")<<" " << request->params("file2_name") << " " << request->params("id1") << " " << request->params("id2") << "\n" ;
}

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    cerr << "Ussage: http_server_example [PORT_NUMBER] [ROOT_DIR]" << endl;
    return EXIT_FAILURE;
  }
  HTTPServer server(stoi(argv[1]));
  Controller::FileController fileController(argv[2]);
  //server.addController<Controller::FileController>("/files/<string:file_path>", fileController);
  //server.addController("/files/<string:file_path>", callback, fileController);
  server.addController("/files/<string:file_path>", &Controller::FileController::callback, &fileController);
  server.addRoute("/test/<string:file_name>/.*/<string:file2_name>/12/<int:id1>/14/a/<int:id2>/.*", testCallback);
  server.addRoute("/MCBs", MCBCallback);
  server.addRoute("/Conductors", conductorsCallback);
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
