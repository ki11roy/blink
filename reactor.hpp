#pragma once

#include <map>
#include <mutex>  
#include <memory>

#include <sys/epoll.h>

#include "handler.hpp"

const int MAX_EVENTS = 1024;

class Reactor
{
  public:
    ~Reactor();
    Reactor();

    void register_handler(HandlerPtr handler);
    
    void resume_handler(HandlerPtr handler);
    
    void remove_handler(HandlerPtr handler);

    void handle_events();
  
  private:
    int epfd_;
    
    struct epoll_event event_;
    
    std::map<int, HandlerPtr> handlers_;
    
    std::mutex mutex_;
};

typedef std::shared_ptr<Reactor> ReactorPtr;

