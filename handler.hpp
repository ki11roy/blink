#pragma once

#include <memory>

enum class State
{
  CLOSE,
  RESUME 
};

class Reactor;
class Handler : public std::enable_shared_from_this<Handler>
{
  public:
    virtual ~Handler();
    Handler();

    Reactor * reactor() const { return reactor_; }

    void reactor(Reactor* reactor) { reactor_ = reactor; }

    virtual int get_handle() const = 0; 

    virtual State handle_input() {}

  private:
    Reactor* reactor_;
};

typedef std::shared_ptr<Handler> HandlerPtr;

