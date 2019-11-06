#include "view_controller.hpp"

#include <inja/inja.hpp>

std::string Controller::ViewController::root_;
std::string Controller::ViewController::layout_;
void Controller::ViewController::init(const std::string& root, const std::string& layout)
{
  root_ = root;
  layout_ = layout;
}

Controller::ViewController::ViewController(const std::string& view): view_(view)
{
}

void Controller::ViewController::render(std::shared_ptr<HTTPRequest> request, const std::string& templateFile, const nlohmann::json& data, const MIMEType& contentType)
{
  inja::Environment env(root_);
  std::string view = env.render_file(view_ + "/" + templateFile, data);
  env.add_callback("view", 0, [&](inja::Arguments& args) ->nlohmann::json {return view;});
  std::string result = env.render_file(layout_, data);
  request->sendResponseHeader(HTTP_OK, contentType);
  request->Send(result.data(), result.length());
}
