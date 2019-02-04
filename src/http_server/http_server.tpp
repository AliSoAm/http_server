template <typename T>
void HTTPServer::addController(const std::string& pattern, T& object)
{
  addRoute(pattern, [&](std::shared_ptr<HTTPRequest> request){object.callback(request);});
}
