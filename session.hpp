#pragma once

#include <memory>

#include "handler.hpp"

#include "sock_stream.hpp"

class Session : public Handler
{
  public:
    ~Session();
    Session();

    SockStream& stream() { return stream_; }

    int get_handle() const { return stream_.get_handle(); }

    State handle_input();

    std::string read_request(unsigned int max_len);

  private:
    SockStream stream_;
    unsigned int timeout_;
};

typedef std::shared_ptr<Session> SessionPtr;

inline bool match_str(const char* b, const char* e, const char* term_str, unsigned int term_len, unsigned int* match_from)
{
    unsigned int n = e - b;
    if( n >= (term_len - *match_from))
    {
        n = (term_len - *match_from);
    }

    unsigned int i;
    for(i = 0; i < n; ++i)
    {
        if( b[i] != term_str[*match_from+i])
        {
            return false;
        }
    }

    if(*match_from + i == term_len)
    {
        return true;
    }
    else
    {
        *match_from += (i);
        return false;
    }
}

