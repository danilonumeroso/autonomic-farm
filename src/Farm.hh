#ifndef FARM_HH_GUARD
#define FARM_HH_GUARD

#include "Emitter.hh"
#include "Collector.hh"
#include "Worker.hh"
#include "Scheduler.hh"
#include "Monitor.hh"
#include <functional>
#include <queue>
#include <condition_variable>

namespace spm {
  std::condition_variable can_emit;
  std::mutex mutex;

  template <class InputType, class OutputType>
  class Farm {

  public:
    typedef Worker<InputType, OutputType> WorkerType;

    Farm(const Farm&) = delete;
    Farm(const Farm&&) = delete;

    Farm(std::vector<InputType>&& stream,
         std::function<OutputType (InputType)> f,
         float ts_goal,
         unsigned nw) : _emitter(new Emitter<InputType>(std::move(stream))),
                        _collector(new Collector<OutputType>()),
                        _scheduler(new SpmScheduler<InputType, OutputType>(f, nw)),
                        _monitor(new Monitor<InputType, OutputType>(_collector.get(), _scheduler.get(), ts_goal))
    { }

    const std::vector<OutputType>& run() {
      _monitor->execute();
      while (!_emitter->is_empty()) {
        WorkerType* worker;

        if ((worker = _scheduler->pick()) == nullptr) {
          std::unique_lock<std::mutex> lock(mutex);
          can_emit.wait(lock);
          continue;
        }

        InputType next = _emitter->emit();
        worker->run(next, _collector.get(), _scheduler.get());
      }

      _monitor->join();
      _scheduler->join();

      return _collector->get_results();
    }

  private:
    std::unique_ptr<Emitter<InputType>> _emitter;
    std::unique_ptr<Collector<OutputType>> _collector;
    std::unique_ptr<Scheduler<InputType, OutputType>> _scheduler;
    std::unique_ptr<Monitor<InputType, OutputType>> _monitor;
  };
}
#endif
