#pragma once

#include <memory>
#include <string>
#include <cstring>

#include "http_server.h"

void fileCallback(std::shared_ptr<HTTPRequest> request);
