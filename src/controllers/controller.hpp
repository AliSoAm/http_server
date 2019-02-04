#ifndef CONTROLLERS_CONTROLLER_H
#define CONTROLLERS_CONTROLLER_H

#include <memory>

#include "http_request.hpp"

namespace Controller
{
  void redirect(std::shared_ptr<HTTPRequest> request, std::string redirectTo);
}

#endif
