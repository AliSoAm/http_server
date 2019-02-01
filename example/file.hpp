#pragma once

#include <memory>
#include <string>

#include "http_server.h"

void fileCB(std::shared_ptr<HTTPRequest> request, const std::string& address, MIMEType type, const std::string& contentEncoding = "");
