#include "reactor.hpp"

#include <memory>

#include <cstring>
#include <string>

#include <unistd.h>
#include <netdb.h>
#include <sys/epoll.h>

#include "exception.hpp"

Reactor::~Reactor()
{
  close(epfd_);
}

Reactor::Reactor()
{
  epfd_ = epoll_create(MAX_EVENTS);
  if(epfd_ == -1) 
  {
    throw exception("Can't create epoll");
  }
}

void Reactor::register_handler(HandlerPtr handler)
{
  std::lock_guard<std::mutex> guard(mutex_);

  struct epoll_event ev;

  ev.events = EPOLLIN | EPOLLONESHOT;
  ev.data.fd = handler->get_handle();

  if(epoll_ctl(epfd_, EPOLL_CTL_ADD, handler->get_handle(), &ev) < 0) 
  {
    throw exception("Can't register handler");
  }

  handler->reactor(this);
  handlers_.insert(std::make_pair(handler->get_handle(), handler));
}

void Reactor::resume_handler(HandlerPtr handler)
{
  std::lock_guard<std::mutex> guard(mutex_);

  struct epoll_event ev;

  ev.events = EPOLLIN | EPOLLONESHOT;
  ev.data.fd = handler->get_handle();

  if(epoll_ctl(epfd_, EPOLL_CTL_MOD, handler->get_handle(), &ev) < 0) 
  {
    throw exception("Can't resume handler");
  }
}

void Reactor::remove_handler(HandlerPtr handler)
{
  std::lock_guard<std::mutex> guard(mutex_);

  struct epoll_event ev;

  ev.events = 0;
  ev.data.fd = handler->get_handle();

  if(epoll_ctl(epfd_, EPOLL_CTL_DEL, handler->get_handle(), &ev) < 0) 
  {
    throw exception("Can't remove handler");
  }

  auto pos = handlers_.find(handler->get_handle());
  if( pos == handlers_.end() )
  {
    throw exception("Can't remove handler, it is not in the list");
  }
  
  handler->reactor(0);
  handlers_.erase(pos);
}

void Reactor::handle_events()
{
  memset(&event_, 0, sizeof(event_));

  for(;;)
  {
    int ndfs = epoll_wait(epfd_, &event_, 1, -1);
    if(ndfs == -1)
    {
      if(errno == EINTR) continue;

      throw exception("Reactor failed on wait with code: ", strerror(errno));
    }

    for(int i = 0; i < ndfs; i++) 
    {
      auto pos = handlers_.find(event_.data.fd);
      if(pos == handlers_.end())
      {
        throw exception("Reactor failed to find handler");
      }
      
      if(event_.events & EPOLLHUP)
      {
        struct epoll_event ev;
        if(epoll_ctl(epfd_, EPOLL_CTL_DEL, event_.data.fd, &ev) < 0)
        {
          throw exception("Reactor failed to delete handler");
        }
      }
      else if(event_.events & EPOLLIN)
      {
        try
        {
          State s = pos->second->handle_input();
          if(s == State::RESUME)
            resume_handler(pos->second);
          else if(s == State::CLOSE)
            remove_handler(pos->second);
        }
        catch(connection_close& e)
        {
          remove_handler(pos->second);
        }
        catch(transport_error& e)
        {
          remove_handler(pos->second);
        }
        catch(std::exception& e)
        {
          remove_handler(pos->second);
          throw e;
        }
      }
    }

  }

}

