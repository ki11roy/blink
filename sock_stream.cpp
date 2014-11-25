#include "sock_stream.hpp"

#include "exception.hpp"

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

SockStream::~SockStream()
{
  ::close(socket_);
}

SockStream::SockStream() : socket_(0)
{
//  if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
//  {
//    throw exception("Can't create socket");
//  }
//  
//  int yes;
//  
//  if(setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
//  {
//    throw exception("Can't set socket options");
//  }
}

int SockStream::recv(void* buf, size_t len, unsigned int timeout)
{
  struct timeval tv;
  
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  
  ::setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
  
  return ::recv(socket_, buf, len, 0);
}

int SockStream::send(const void* buf, size_t len)
{
  return ::send(socket_, buf, len, 0);
}

