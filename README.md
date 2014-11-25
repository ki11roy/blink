Blink
=====

Basic epoll server written in C++

##Build
```
cmake .
make
./blink
```

##Test
* Open up a browser and type: http://localhost/big.txt
* Run a multithreaded ./test.rb

##Limitations
* processes only GET requests
* no accept timeout
