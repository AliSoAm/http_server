#ifndef CONTROLLERS_VIEW_CONTROLLER_H
#define CONTROLLERS_VIEW_CONTROLLER_H

#include <string>

#include <nlohmann/json.hpp>

#include "controller.hpp"

namespace Controller
{
  class ViewController
  {
  public:
    static void init(const std::string& root, const std::string& layout);
    ViewController(const std::string& view);
  protected:
    void render(std::shared_ptr<HTTPRequest> request, const std::string& templateFile, const nlohmann::json& arguments = "", const MIMEType& contentType = MIME_TEXT_HTML);
    static std::string root_;
    static std::string layout_;
    std::string view_;
  };
}

#endif
