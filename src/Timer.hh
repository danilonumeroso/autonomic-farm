#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

class Timer {
private:
  std::chrono::system_clock::time_point _start;
  std::chrono::system_clock::time_point _stop;
  std::string _name;
  using usecs = std::chrono::microseconds;
  using msecs = std::chrono::milliseconds;

public:
  Timer(std::string name) : _name(name) {
    _start = std::chrono::system_clock::now();
  }

  ~Timer() {
    _stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = _stop - _start;
    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    std::cout << _name << " computed in " << musec << " usec " << std::endl;
  }
};

#endif
