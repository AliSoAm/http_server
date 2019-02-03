#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H

#include <memory>

#include "http_request.hpp"

class BaseController
{
public:
private:
  static void redirect(std::shared_ptr<HTTPRequest>, std::string redirectTo);
};

#endif
