#ifndef File_CONTROLLER_H
#define File_CONTROLLER_H

#include <string>
#include <fstream>

#include "controller.hpp"

namespace Controller
{
  class FileController
  {
  public:
    FileController() = delete;
    static void setRoot(const std::string& root);
    static void callback(std::shared_ptr<HTTPRequest>);
  private:
    static bool fileExist(const std::string& path);
    static MIMEType getMimeType(const std::string& path);
    static std::string root_;
  };
}

#endif
