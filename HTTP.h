#ifndef HTTP_H
#define HTTP_H
#include <exception>
#include <cstring>

enum HTTPMethod
{
    HTTP_METHOD_NOT_SPECIFIED,
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
};


enum HTTPCompression
{
    HTTP_COMPRESSION_NOT_SPECIFIED,
    HTTP_COMPRESSION_DEFLATE,
    HTTP_COMPRESSION_GZIP,
};

enum MIMEType
{
    MIME_TYPE_NOT_SPECIFIED = 0,
    MIME_ALL,
    MIME_APPLICATION_JAVASCRIPT,
    MIME_APLICATION_JSON,
    MIME_APPLICATION_X_WWW_FORM_URLENCODED,
    MIME_APPLICATION_XML,
    MIME_APPLICATION_ZIP,
    MIME_APPLICATION_PDF,
    MIME_AUDIO_MPEG,
    MIME_AUDIO_vorbis,
    MIME_MULTIPART_FORM_DATA,
    MIME_TEXT_CSS,
    MIME_TEXT_HTML,
    MIME_TEXT_PLAIN,
    MIME_IMAGE_PNG,
    MIME_IMAGE_JPEG,
    MIME_IMAGE_GIF,
};


#define CONNECTION_CLOSE                            "close"
#define CONNECTION_KEEP_ALIVE                       "keep-alive"
#define CONNECTION_UPGRADE                          "upgrade"

enum HTTPConnection
{
    HTTP_CONNECTION_CLOSE,
    HTTP_CONNECTION_NOT_SPECIFIED,
    HTTP_CONNECTION_KEEP_ALIVE,
    HTTP_CONNECTION_UPGRADE,
};

#define TRANSFER_ENCODING_CHUNKED                   "chunked"
#define TRANSFER_ENCODING_IDENTITY                  "dentity"
enum HTTPTransferEncoding
{
    HTTP_TRANSFER_ENCODING_NOT_SPECIFIED,
    HTTP_TRANSFER_ENCODING_CHUNKED,
    HTTP_TRANSFER_ENCODING_IDENTITY,
};

#define HTTP_OK                                     200

#define HTTP_NOT_MODIFIED                           304

#define HTTP_BAD_REQUEST                            400
#define HTTP_FORBIDDEN                              403
#define HTTP_NOT_FOUND                              404
#define HTTP_METHOD_NOT_ALLOWED                     405
#define HTTP_REQUEST_URI_TOO_LONG                   414
#define HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE        431

#define HTTP_INTERNAL_SERVER_ERROR                  500
#define HTTP_NOT_IMPLEMENTED                        501



class HTTPException: public std::exception
{
public:
    HTTPException(int HTTPErrorCode, const char* what): HTTPErrorCode_(HTTPErrorCode)
    {
        std::strncpy(what_, what, 29);
        what_[29] = '\0';
    }
    HTTPException(int HTTPErrorCode): HTTPErrorCode_(HTTPErrorCode)
    {
        what_[0] = '\0';
    }
    int HTTPErrorCode() const
    {
        return HTTPErrorCode_;
    }
    virtual const char* what() const throw()
    {
        return what_;
    }

private:
    int HTTPErrorCode_;
    char what_[30];
};

#endif
