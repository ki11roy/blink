#include "reactor.hpp"

#include <vector>
#include <thread>
#include <algorithm>

#include <signal.h>

#include "debug.hpp"

#include "acceptor.hpp"

const int SESSION_THREAD_COUNT = 2;

int main()
{
  DEBUG("[STARTUP]");

  signal(SIGPIPE, SIG_IGN);

  ReactorPtr acceptor_reactor(new Reactor());
  ReactorPtr session_reactor(new Reactor());

  AcceptorPtr acceptor = Acceptor::create(acceptor_reactor, session_reactor);
  
  acceptor->open();

  std::thread acceptor_thread([&]() 
      {
      DEBUG("[acceptor_event_loop] STARTUP");
      acceptor_reactor->handle_events();
      DEBUG("[acceptor_event_loop] SHUTDOWN");
      });


  std::vector<std::thread> workers;
  for (int i = 0; i < SESSION_THREAD_COUNT; i++) {
    workers.push_back(std::thread([&]() 
          {
          DEBUG("[session_event_loop] STARTUP");
          session_reactor->handle_events();
          DEBUG("[session_event_loop] SHUTDOWN");
          }));
  }

  acceptor_thread.join();

  std::for_each(workers.begin(), workers.end(), [](std::thread &t) 
  {
      t.join();
  });
  
  DEBUG("[SHUTDOWN]");
    
  return 0;
}
