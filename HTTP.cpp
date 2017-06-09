#include "HTTP.h"
#include <map>

using namespace std;

struct HTTPResponseCodePair
{
    unsigned int code;
    const char* response;
};
const static struct HTTPResponseCodePair HTTPResponseCodeMap[] =
{
    {HTTP_OK                                , "OK"                              },
    {HTTP_NOT_MODIFIED                      , "NOT MODIFIED"                    },
    {HTTP_BAD_REQUEST                       , "BAD REQUEST"                     },
    {HTTP_FORBIDDEN                         , "HTTP FORBIDDEN"                  },
    {HTTP_NOT_FOUND                         , "NOT FOUND"                       },
    {HTTP_METHOD_NOT_ALLOWED                , "METHOD NOT ALLOWED"              },
    {HTTP_REQUEST_URI_TOO_LONG              , "REQUEST URI TOO LONG"            },
    {HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE   , "REQUEST HEADER FIELDS TOO LARGE" },
    {HTTP_INTERNAL_SERVER_ERROR             , "INTERNAL SERVER ERROR"           },
    {HTTP_NOT_IMPLEMENTED                   , "NOT IMPLEMENTED"                 },
};

string getHTTPResponseMessage(unsigned int code)
{

    for (size_t i = 0; i < (sizeof(HTTPResponseCodeMap) / sizeof(struct HTTPResponseCodePair)); i++)
        if (HTTPResponseCodeMap[i].code == code)
            return HTTPResponseCodeMap[i].response;
    return "";
}

HTTPException::HTTPException(unsigned int HTTPErrorCode, const char* what): HTTPErrorCode_(HTTPErrorCode)
{
    strncpy(what_, what, 49);
    what_[49] = '\0';
}

HTTPException::HTTPException(unsigned int HTTPErrorCode): HTTPErrorCode_(HTTPErrorCode)
{
    strncpy(what_, getHTTPResponseMessage(HTTPErrorCode).c_str(), 49);
    what_[49] = '\0';
}

int HTTPException::HTTPErrorCode() const
{
    return HTTPErrorCode_;
}

const char* HTTPException::what() const noexcept
{
    return what_;
}
