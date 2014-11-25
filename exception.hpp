#include <string>
#include <stdexcept>

class exception : public std::runtime_error
{
  public:
    exception(const std::string& msg) : std::runtime_error(msg)
  {
  }

    exception(const std::string& msg1, const std::string& msg2)
      : std::runtime_error(msg1 + " "+ msg2)
    {}

    exception(const std::string& msg1, const std::string& msg2, const std::string& msg3)
      : std::runtime_error(msg1 + " "+ msg2 + " " + msg3)
    {}
};

class connection_close : public exception
{
  public:
    connection_close(const std::string& msg) : exception(msg)
  {
  }
};

class transport_error : public exception
{
  public:
    transport_error(const std::string& msg) 
      : exception("Transport error: " + msg)
    {}
};

class protocol_error : public exception 
{
  public:
    protocol_error(const std::string& retmsg) 
      : exception("Protocol error [" + retmsg + "]")
    {}
    protocol_error(const std::string& cmd, const std::string& retmsg) 
      : exception("Protocol error [" + cmd + "] [" + retmsg + "]")
    {}
    protocol_error(const std::string& cmd, const std::string& arg, const std::string& retmsg) 
      : exception("Protocol error [" + cmd + " " + arg + "] [" + retmsg + "]")
    {}
};

class not_found : public exception
{
  public:
    not_found(const std::string& msg) 
      : exception(msg)
    {}
};

