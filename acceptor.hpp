#pragma once

#include <memory>

#include "handler.hpp"
#include "reactor.hpp"

class Acceptor;
typedef std::shared_ptr<Acceptor> AcceptorPtr;

class SockStream;
class Acceptor : public Handler
{
  private:
    Acceptor(ReactorPtr acceptor_reactor, ReactorPtr session_reactor);

  public:
    ~Acceptor();
    
    Acceptor& operator=( const Acceptor & ) = delete;
    Acceptor( const Acceptor & ) = delete;

    static AcceptorPtr create(ReactorPtr acceptor_reactor, ReactorPtr session_reactor) {return AcceptorPtr(new Acceptor(acceptor_reactor, session_reactor)); }

    void open();

    void accept(SockStream& stream);
    void close();

    State handle_input();

    int get_handle() const { return socket_; }

  private:
    int socket_;

    ReactorPtr acceptor_reactor_;
    ReactorPtr session_reactor_;

    unsigned int timeout_;
};


