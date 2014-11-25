#pragma once

#include <vector>
#include <string>

const int BUFF_SIZE = 1024;

class SockStream
{
  public:
    ~SockStream();
    SockStream();

    int get_handle() const { return socket_; }
    void set_handle(int socket) { socket_ = socket; }

    int recv(void* buf, size_t len, unsigned int timeout);
    int send(const void* buf, size_t len);

  private:
    int socket_;
};
