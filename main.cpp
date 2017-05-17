#include <iostream>
#include "HTTPServer.h"

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
        return EXIT_FAILURE;
    HTTPServer server(atoi(argv[1]));
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
