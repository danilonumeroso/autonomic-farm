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
  std::mutex modify_workers_queue;
  int _worker_id = 0;

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
        _f(f)
    {
      for (unsigned i=0; i < nw; ++i) {
        auto w = new Worker<InputType, OutputType>(f, ++_worker_id);
        _all_workers.push_back(w);
        _sleeping_workers.push_back(w);
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

      pointer worker = _sleeping_workers.back();
      _sleeping_workers.pop_back();

      return worker;
    }

    void done(pointer worker) {
      {
        std::unique_lock<std::mutex> lock(modify_workers_queue);
        _sleeping_workers.push_back(worker);
      }
      can_emit.notify_one();
    }

    void add_worker(unsigned n) {
      std::unique_lock<std::mutex> lock(modify_workers_queue);
      for (int i = 0; i < n; ++i) {
        auto w = new Worker<InputType, OutputType>(_f, ++_worker_id);
        _sleeping_workers.push_back(w);
        _all_workers.push_back(w);
      }
    }

    void remove_worker() {
      std::unique_lock<std::mutex> lock(modify_workers_queue);

      if (_sleeping_workers.empty() || _all_workers.size() == 1) {
        return;
      }

      auto w = _sleeping_workers.back();
      _sleeping_workers.pop_back();

      for (auto i = _all_workers.begin(), end = _all_workers.end(); i < end; ++i) {
        if (*i == w) {
          _all_workers.erase(i);
          break;
        }
      }
    }

    void join() {
      int h = 0;
      for (auto i = _all_workers.begin(), end = _all_workers.end(); i < end; ++i) {
        (*i)->join();
      }
    }

  private:
    std::vector<pointer> _all_workers;
    std::vector<pointer> _sleeping_workers;
    std::function<OutputType (InputType)> _f;
  };
}
#endif
