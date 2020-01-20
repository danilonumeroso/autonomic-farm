#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include "Constant.hh"

template<class T>
std::string time_type() = delete;

template<>
std::string time_type<std::chrono::microseconds>() { return "us"; }

template<>
std::string time_type<std::chrono::milliseconds>() { return "ms"; }

template<>
std::string time_type<std::chrono::seconds>() { return "s"; }

template <class UnitOfTime>
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
    auto musec = std::chrono::duration_cast<UnitOfTime>(elapsed).count();

    REPORT(std::to_string(musec).append(time_type<UnitOfTime>()));

    std::clog << _name << " computed in " << musec << time_type<UnitOfTime>() << std::endl;
  }
};

#endif
