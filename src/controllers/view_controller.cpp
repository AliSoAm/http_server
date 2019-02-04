#include "view_controller.hpp"

std::string Controller::ViewController::root_;
std::string Controller::ViewController::layout_;
void Controller::ViewController::init(const std::string& root, const std::string& layout)
{

}

void Controller::ViewController::render(const std::string& templateFile, const nlohmann::json& arguments)
{

}

Controller::ViewController::ViewController(const std::string& view): view_(view)
{

}
