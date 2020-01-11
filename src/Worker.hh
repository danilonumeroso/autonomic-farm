#ifndef WORKER_HH_GUARD
#define WORKER_HH_GUARD

#include <thread>
#include <functional>
#include "Collector.hh"

template <class InputType, class OutputType>
class Worker {
private:
  std::function<OutputType (InputType)> f;
  std::thread* t;
public:
  Worker(std::function<OutputType (InputType)> f)
    : f(f) { }

  void operator()(InputType x, const Collector<OutputType>& collector) {
    collector.push_back(f(x));
  }
};

#endif
