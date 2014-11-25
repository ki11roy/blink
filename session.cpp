#include "session.hpp"

#include <string>
#include <sstream>

#include <sys/sendfile.h>  // sendfile
#include <fcntl.h>         // open
#include <unistd.h>        // close
#include <sys/stat.h>      // fstat
#include <sys/types.h>     // fstat

#include "debug.hpp"
#include "exception.hpp"
#include "response.hpp"

Session::~Session()
{
//  DEBUG("~Session");
}

Session::Session() : timeout_(3)
{
//  DEBUG("Session");
}

std::string Session::read_request(unsigned int max_len)
{
    std::vector<char> buff(max_len);

    unsigned int read_bytes = 0;
    unsigned int match_from = 0;

    while( read_bytes < max_len)
    {
        int received = stream_.recv(&buff[read_bytes], max_len - read_bytes, timeout_);
        if(received == -1)
        {
            throw transport_error("Can't receive data");
        }
        else if(received == 0)
        {
            throw transport_error("Can't receive data");
        }

        unsigned int line_start = read_bytes;
        for(unsigned int i = read_bytes; i < read_bytes + received; ++i)
        {
            if(match_str(&buff[i], &buff[received], "\r\n\r\n", 4, &match_from ))
            {
                if(std::string(&buff[0], 3) != "GET")
                  throw protocol_error("Unsupported method");

                // let's compiler optimizes it itself 
                return std::string( &buff[0], read_bytes + received);
            }
        }

        read_bytes += received;
    }
    
    throw protocol_error("Broken request");
}

State Session::handle_input()
{
  try
  {
    std::string request = read_request(4096);
    
    std::string method;
    std::string url;
    
    std::stringstream ss(request);
    ss >> method;
    ss >> url;

    // remove leading slash
    if(!url.empty())
      url.erase(0, 1);

    DEBUG("Request: %s", url.c_str());
    
    int source = open(url.c_str(), O_RDONLY, 0);
    if(source < 0)
    {
      throw not_found("File not found");
    }

    struct stat stat_source;
    fstat(source, &stat_source);
    
    std::stringstream reply;
    reply << "HTTP/1.1 200 OK" << "\r\n";
    reply << "Content-Type: text/plain; charset=utf-8" << "\r\n";
    reply << "Content-Length: " << stat_source.st_size << "\r\n";
    reply << "\r\n";

    std::string reply_str(reply.str());

    stream_.send(reply_str.c_str(), reply_str.length());

    sendfile(stream_.get_handle(), source, 0, stat_source.st_size);    
   
    close(source); 
  }
  catch(protocol_error& e)
  {
    Response r(400, "Bad request");
    std::string reply_str(r.to_string());
    stream_.send(reply_str.c_str(), reply_str.length());
  }
  catch(not_found& e)
  {
    Response r(404, "Not found");
    std::string reply_str(r.to_string());
    stream_.send(reply_str.c_str(), reply_str.length());
  }

  return State::CLOSE;
}

