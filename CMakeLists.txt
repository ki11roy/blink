CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
PROJECT(BLINK)

SET(CMAKE_THREAD_PREFER_PTHREAD TRUE)

FIND_PACKAGE(Threads)

IF(CMAKE_USE_PTHREADS_INIT)
  LIST(APPEND CMAKE_CXX_FLAGS "-pthread -std=c++0x ${CMAKE_CXX_FLAGS} -g")
ENDIF()

SET(BLINK
  blink.cpp
  reactor.cpp
  handler.cpp
  sock_stream.cpp
  acceptor.cpp
  session.cpp
  )

ADD_EXECUTABLE(blink ${BLINK})
TARGET_LINK_LIBRARIES(blink ${CMAKE_THREAD_LIBS_INIT})

