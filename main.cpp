#include <iostream>
#include "HTTPServer.h"

using namespace std;

void indexCB(HTTPRequest& request)
{
    cout << "Index" << endl;
    char* buffer = new char[10000];
    int len = request.Recv(buffer, 9999);
    buffer[len] = 0;
    cout << "HTTP recv:" << endl;
    cout << "\tbufferLen: \"" << len << "\"" << endl;
    cout << "\tbuffer: \"" << buffer << "\"" << endl;

}

void testCB(HTTPRequest& request)
{
    cout << "Test" << endl;
}

int main(int argc, char** argv)
{

    if (argc < 2)
        return EXIT_FAILURE;
    HTTPServer server(atoi(argv[1]));
    server.setRootCallback(indexCB);
    server.addRootURI("Test", testCB);
    try
    {
        server.loop();
    }
    catch (HTTPException& e)
    {
        cout << e.what();
    }
    return EXIT_SUCCESS;
}
