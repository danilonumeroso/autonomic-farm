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
  std::mutex modify_reservoir;

  int _worker_id = 0;

  template <class InputType, class OutputType>
  class Farm;

  template <class InputType, class OutputType>
  class Worker;

  template <class InputType, class OutputType>
  class Scheduler {

  public:
    typedef Worker<InputType, OutputType> WorkerType;

    Scheduler(Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;

    Scheduler(std::function<OutputType (InputType)> f,
              unsigned nw)
      : _all_workers(),
        _sleeping_workers(),
        _reservoir(),
        _f(f)
    {
      for (unsigned i=0; i < nw; ++i) {
        WorkerType* w = new Worker<InputType, OutputType>(f, ++_worker_id);
        _all_workers.push_back(w);
        _sleeping_workers.push_back(w);
      }

      assert(_sleeping_workers.size() == nw);
    }

    ~Scheduler() {
      for (auto i = _all_workers.begin(), end = _all_workers.end(); i < end; ++i) {
        delete *i;
      }
    }

    int no_workers() {
      return _all_workers.size();
    }

    WorkerType* pick() {
      if (_sleeping_workers.empty()) {
        return nullptr;
      }

      auto worker = _sleeping_workers.back();
      _sleeping_workers.pop_back();

      return worker;
    }

    void done(WorkerType* worker) {
      if (worker->will_terminate()) {
        return;
      }
      std::unique_lock<std::mutex> lock(modify_workers_queue);

      _sleeping_workers.push_back(worker);
      can_emit.notify_one();

    }

    void add_worker(unsigned n) {
      std::unique_lock<std::mutex> lock1(modify_workers_queue);
      std::unique_lock<std::mutex> lock2(modify_reservoir);

      unsigned added_from_reservoir = 0U;

      while(!_reservoir.empty() && added_from_reservoir < n) {
        auto w = _reservoir.back();
        _reservoir.pop_back();
        _sleeping_workers.push_back(w);
      }

      for (unsigned i = 0; i < (n-added_from_reservoir); ++i) {
        auto w = new WorkerType(_f, ++_worker_id);
        _sleeping_workers.push_back(w);
        _all_workers.push_back(w);
      }
    }

    void remove_worker(unsigned n) {
      std::unique_lock<std::mutex> lock(modify_reservoir);

      if (n >= _all_workers.size()) {
        n = _all_workers.size() - 1;
      }

      for (unsigned i = 0; i < n; ++i) {
        auto w = _all_workers.back();
         w->drop();
        _reservoir.push_back(w);
      }
    }

    void join() {
      for (auto i = _all_workers.begin(), end = _all_workers.end(); i < end; ++i) {
        (*i)->join();
      }
    }

  private:
    std::vector<WorkerType*> _all_workers;
    std::vector<WorkerType*> _sleeping_workers;
    std::vector<WorkerType*> _reservoir;

    std::function<OutputType (InputType)> _f;
  };
}
#endif
