#ifndef FARM_HH_GUARD
#define FARM_HH_GUARD

#include "Emitter.hh"
#include "Collector.hh"
#include "Worker.hh"
#include "Scheduler.hh"
#include <functional>
#include <queue>
#include <condition_variable>

namespace spm {
  std::condition_variable can_emit;
  std::mutex mutex;

  template <class InputType, class OutputType>
  class Farm {

  public:
    typedef Worker<InputType, OutputType> worker_type;

    Farm(const Farm&) = delete;
    Farm(const Farm&&) = delete;

    Farm(std::initializer_list<InputType> stream,
         std::function<OutputType (InputType)> f,
         unsigned nw) : _emitter(new Emitter<InputType>(stream)),
                        _collector(new Collector<OutputType>()),
                        _scheduler(new Scheduler<InputType, OutputType>(f, nw))
    { }

    ~Farm() {
      delete _scheduler;
      delete _collector;
      delete _emitter;
    }

    const std::vector<OutputType>& run() {

      while (!_emitter->is_empty()) {
        worker_type* worker;

        if ((worker = _scheduler->pick()) == nullptr) {
          std::unique_lock<std::mutex> lock(mutex);
          can_emit.wait(lock);
          continue;
        }

        InputType next = _emitter->emit();
        worker->exec(next, _collector, _scheduler);
      }
      _scheduler->join();
      return _collector->get_results();
    }

  private:
    Emitter<InputType>* _emitter;
    Collector<OutputType>* _collector;
    Scheduler<InputType, OutputType>* _scheduler;
    // Monitor
  };
}
#endif
