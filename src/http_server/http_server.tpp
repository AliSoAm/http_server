template <typename T>
void HTTPServer::addController(const std::string& pattern, void (T::*callback)(std::shared_ptr<HTTPRequest>), T* t)
{
  addRoute(pattern, [=](std::shared_ptr<HTTPRequest> request){(t->*callback)(request);});
}
