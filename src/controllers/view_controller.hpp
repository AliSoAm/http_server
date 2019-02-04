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
    static init(const std::string& root, const std::string& layout);
    ViewController(const std::string& view);
  private:
    void render(const nlohmann::json& arguments);
    static std::string root_;
    static std::string layout_;
    std::string view_;
  };
}

#endif
