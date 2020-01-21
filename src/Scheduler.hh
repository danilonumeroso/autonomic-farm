#ifndef SCHEDULER_HH_GUARD
#define SCHEDULER_HH_GUARD

#include "Worker.hh"
#include "Emitter.hh"
#include "Timer.hh"
#include <functional>
#include <queue>
#include <assert.h>
#include <condition_variable>
#include <algorithm>
#include "Synchronization.hh"

namespace spm {
  int _worker_id = 0;

  template <class InputType, class OutputType>
  class Worker;

  template <class InputType, class OutputType>
  class SpmWorker;

  template <class InputType, class OutputType>
  class Scheduler {
  public:
    typedef Worker<InputType, OutputType> WorkerType;
    typedef std::unique_ptr<WorkerType> UniquePtr;
    typedef std::function<OutputType (InputType)> Task;
    typedef unsigned Integer;

    Scheduler() = default;

    Scheduler(Task t) : workers(), sleeping_workers(),
                        reservoir(), task(t)
    { }

    Integer get_no_active_workers() {
      return std::count_if(workers.cbegin(),
                           workers.cend(),
                           [](const UniquePtr& w) {
                             return !w->will_terminate();
                           });
    }

    virtual void add_worker(unsigned) = 0;
    virtual void remove_worker(unsigned) = 0;
    virtual WorkerType* pick() = 0;
    virtual void join() = 0;
    virtual void done(WorkerType*) = 0;

  protected:
    std::vector<UniquePtr> workers;
    std::vector<WorkerType*> sleeping_workers;
    std::vector<WorkerType*> reservoir;
    Task task;
  };

  template <class InputType, class OutputType>
  class SpmScheduler : public Scheduler<InputType, OutputType> {
    using Base = Scheduler<InputType, OutputType>;

  public:
    typedef SpmWorker<InputType, OutputType> WorkerType;
    typedef typename Base::UniquePtr UniquePtr;
    typedef typename Base::Task Task;
    typedef typename Base::Integer Integer;

    SpmScheduler(Task t, Integer nw)
      : Base(t)
    {
       for (Integer i = 0; i < nw; ++i) {
        UniquePtr w(new WorkerType(t, ++_worker_id));
        Base::sleeping_workers.push_back(w.get());
        Base::workers.push_back(std::move(w));
       }
    }

    virtual typename Base::WorkerType* pick() {
      if (Base::sleeping_workers.empty()) {
        return nullptr;
      }

      auto worker = Base::sleeping_workers.back();
      Base::sleeping_workers.pop_back();

      return worker;
    }

    virtual void done(typename Base::WorkerType* w) {
      if (w->will_terminate()) {
        return;
      }

      __LOCK_SLEEPING_WORKERS_QUEUE__

      Base::sleeping_workers.push_back(w);

      __NOTIFY_EMITTER__;
    }

    virtual void add_worker(unsigned n) {
      __LOCK_SLEEPING_WORKERS_QUEUE__
      __LOCK_RESERVOIR__

      Integer added_from_reservoir = 0U;

      while(!Base::reservoir.empty() && added_from_reservoir < n) {

        auto w = Base::reservoir.back();
        Base::reservoir.pop_back();

        w->revive();
        Base::sleeping_workers.push_back(w);

        ++added_from_reservoir;
      }

      for (unsigned i = 0; i < (n-added_from_reservoir); ++i) {
        UniquePtr w(new WorkerType(Base::task, ++_worker_id));
        Base::sleeping_workers.push_back(w.get());
        Base::workers.push_back(std::move(w));
      }
    }

    virtual void remove_worker(Integer n) {
      __LOCK_RESERVOIR__

      if (n >= Base::workers.size()) {
        n = Base::workers.size() - 1;
      }

      for (Integer i = 0; i < n; ++i) {
        auto w = std::move(Base::workers.back());
        Base::workers.pop_back();

        w->terminate();
        Base::reservoir.push_back(w.get());

        Base::workers.insert(Base::workers.begin(), std::move(w));
      }
    }

    virtual void join() {
      for (const auto& i: Base::workers) {
        i->join();
      }
    }
  };
}
#endif
