

#include "HTTP.h"
#include <map>

using namespace std;
const static map<unsigned int, string> HTTPResponseCodeMap =
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

const string& getHTTPResponseMessage(unsigned int code)
{
    try
    {
        return HTTPResponseCodeMap.at(code);
    }
    catch (...)
    {
        throw HTTPException(HTTP_INTERNAL_SERVER_ERROR);
    }
}

HTTPException::HTTPException(int HTTPErrorCode, const char* what): HTTPErrorCode_(HTTPErrorCode)
{
    strncpy(what_, what, 49);
    what_[49] = '\0';
}
HTTPException::HTTPException(int HTTPErrorCode): HTTPErrorCode_(HTTPErrorCode)
{
    strncpy(what_, getHTTPResponseMessage(HTTPErrorCode).c_str(), 49);
    what_[49] = '\0';
}

int HTTPException::HTTPErrorCode() const
{
    return HTTPErrorCode_;
}

const char* HTTPException::what() const throw()
{
    return what_;
}
