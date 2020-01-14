#ifndef WORKER_HH_GUARD
#define WORKER_HH_GUARD

#include <thread>
#include <functional>
#include "Collector.hh"
#include "Scheduler.hh"
#include "Timer.hh"
#include <iostream>
#include <mutex>
#include <condition_variable>

namespace spm {
  std::mutex write_to_output_stream;

  template <class InputType, class OutputType>
  class Worker {
  private:
    std::function<OutputType (InputType)> f;
    std::thread* t;
  public:

    Worker(std::function<OutputType (InputType)> f)
      : f(f)
    { }

    Worker(const Worker&) = delete;
    Worker(const Worker&&) = delete;

    ~Worker() {
      delete t;
    }

    void exec(InputType x, Collector<OutputType>* collector,
              Scheduler<InputType, OutputType>* scheduler) {

      t = new std::thread([x, this, collector, scheduler] {
                            Timer t("Step (" + std::to_string(x) + ")");
                            InputType res = f(x);
                            {
                              std::unique_lock<std::mutex> lock(write_to_output_stream);
                              collector->push_back(res);
                            }
                            scheduler->done(this);
                          });
    }

    void join() {
      if (t == nullptr) {
        return;
      }
      t->join();
    }
  };
}
#endif
