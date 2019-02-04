#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>

#include "http_request.hpp"

namespace Controller
{
  void redirect(std::shared_ptr<HTTPRequest> request, std::string redirectTo);
}

#endif
