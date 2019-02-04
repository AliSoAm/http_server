#ifndef CONTROLLERS_File_CONTROLLER_H
#define CONTROLLERS_File_CONTROLLER_H

#include <string>
#include <fstream>

#include "controller.hpp"

namespace Controller
{
  class FileController
  {
  public:
    FileController(const std::string& root);
    void callback(std::shared_ptr<HTTPRequest>);
  private:
    bool fileExist(const std::string& path);
    MIMEType getMimeType(const std::string& path);
    std::string root_;
  };
}

#endif
