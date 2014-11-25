#pragma once

#include <sstream>

class Response
{
  public:
    ~Response()
    {

    }
    
    Response(int code, const std::string& text) : code_(code), text_(text)
    {
    }

    std::string to_string()
    {
      std::string body = text_ + "\r\n";
      std::stringstream reply;
      reply << "HTTP/1.1 "<< code_ << " " << body;
      reply << "Content-Type: text/html; charset=utf-8" << "\r\n";
      reply << "Content-Length: " << body.length() << "\r\n";
      reply << "\r\n";
      reply << body;
      return reply.str();
    }

  private:
    int code_;
    std::string text_;

};
