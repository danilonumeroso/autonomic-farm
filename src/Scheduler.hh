#ifndef SCHEDULER_HH_GUARD
#define SCHEDULER_HH_GUARD

#include "Worker.hh"
#include "Emitter.hh"
#include "Farm.hh"
#include "Timer.hh"
#include <functional>
#include <queue>
#include <assert.h>
#include <iostream>
#include <condition_variable>

namespace spm {
  extern std::condition_variable can_emit;

  template <class InputType, class OutputType>
  class Farm;

  template <class InputType, class OutputType>
  class Worker;

  template <class InputType, class OutputType>
  class Scheduler {

  public:
    typedef Worker<InputType, OutputType> worker_type;
    typedef worker_type& reference;
    typedef const worker_type& const_reference;
    typedef worker_type* pointer;

    Scheduler(Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;

    Scheduler(std::function<OutputType (InputType)> f,
              unsigned nw)
      : _all_workers(),
        _sleeping_workers(),
        _more_workers()
    {
      for (unsigned i=0; i < nw; ++i) {
        auto w = new Worker<InputType, OutputType>(f);
        _all_workers.push_back(w);
        _sleeping_workers.push(w);
      }

      assert(_sleeping_workers.size() == nw);
    }

    ~Scheduler() {
      for (auto i = _all_workers.begin(), end = _all_workers.end();
           i < end; ++i) {
        delete *i;
      }
    }

    pointer pick() {
      if (_sleeping_workers.empty()) {
        return nullptr;
      }

      pointer worker = _sleeping_workers.front();
      _sleeping_workers.pop();

      return worker;
    }

    void done(pointer worker) {
      can_emit.notify_one();
      _sleeping_workers.push(worker);
    }

    void join() {
      for (auto i = _all_workers.begin(), end = _all_workers.end(); i < end; ++i) {
        (*i)->join();
      }
    }

  private:
    std::vector<pointer> _all_workers;
    std::queue<pointer> _sleeping_workers;
    std::queue<pointer> _more_workers;
  };
}
#endif
