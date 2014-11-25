#include "acceptor.hpp"

#include <memory>

#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "sock_stream.hpp"
#include "reactor.hpp"
#include "exception.hpp"
#include "config.hpp"
#include "debug.hpp"
#include "session.hpp"

Acceptor::~Acceptor()
{
  ::close(socket_);
}

Acceptor::Acceptor(ReactorPtr acceptor_reactor, ReactorPtr session_reactor) : 
  acceptor_reactor_(acceptor_reactor),
  session_reactor_(session_reactor),
  timeout_(3)
{
}

void Acceptor::open()
{
  if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    throw exception("Can't create socket");
  }
  
  int yes;
  
  if(setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    throw exception("Can't set socket options");
  }
  
  
  sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  serveraddr.sin_port = htons(PORT);
  
  memset(&(serveraddr.sin_zero), '\0', 8);

  if(bind(socket_, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
  {
    throw exception("Can't bind to the socket");
  }
  if(listen(socket_, SOMAXCONN) == -1)
  {
    throw exception("Can't listen on the socket");
  }
  struct timeval tv;
  
  tv.tv_sec = timeout_;
  tv.tv_usec = 0;
  
  setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

//  acceptor_reactor_->register_handler(this->get_ptr());
  acceptor_reactor_->register_handler(shared_from_this());
}

void Acceptor::accept(SockStream& stream)
{
  int socket;
  struct sockaddr_in clientaddr;
  unsigned int addrlen = sizeof(clientaddr);
  
  if((socket = ::accept(socket_, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
  {
    throw exception("Can't accept connection");
  }

  stream.set_handle(socket);
}

void Acceptor::close()
{

}

State Acceptor::handle_input()
{
//  DEBUG("acceptor handle input");

  SessionPtr session(new Session());

  accept(session->stream());

  session_reactor_->register_handler(session);
  
  return State::RESUME;
//  DEBUG("accepted");
}
