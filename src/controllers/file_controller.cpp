#include "file_controller.hpp"

#include <array>
#include <regex>
#include <vector>
#include <utility>

#ifndef FILE_CONTROLLER_BUFFER_SIZE
# define FILE_CONTROLLER_BUFFER_SIZE   1024
#endif

std::string Controller::FileController::root_;

void Controller::FileController::setRoot(const std::string& root)
{
  root_ = root;
}

void Controller::FileController::callback(std::shared_ptr<HTTPRequest> request)
{
  std::ifstream file;
  file.exceptions(std::ifstream::badbit);
  std::string filePath = root_ + "/" + request->params("file_path").get<std::string>();
  if (fileExist(filePath + ".gz"))
  {
    request->addResponseHeaderParameter("content-encoding", "gzip");
    filePath += ".gz";
  }
  else if (!fileExist(filePath))
    throw HTTPException(HTTP_NOT_FOUND);
  file.open(filePath);
  request->sendResponseHeader(HTTP_OK, getMimeType(filePath));
  while (!file.eof())
  {
    std::array<char, FILE_CONTROLLER_BUFFER_SIZE> buffer;
    file.read(buffer.data(), buffer.size());
    request->Send(buffer.data(), file.gcount());
  }
}

bool Controller::FileController::fileExist(const std::string& path)
{
  std::ifstream infile(path);
  return infile.good();
}

MIMEType Controller::FileController::getMimeType(const std::string& path)
{
  std::vector<std::pair<const char*, MIMEType>> mimes({{"css", MIME_TEXT_CSS}, {"js", MIME_APPLICATION_JAVASCRIPT}, {"html", MIME_TEXT_HTML}});
  for (auto& mime: mimes)
    if (std::regex_match(path, std::regex(std::string("(.+)\\.") + mime.first + "(\\.gz)?$")))
      return mime.second;
  return MIME_TYPE_NOT_SPECIFIED;
}
