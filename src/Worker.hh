#ifndef WORKER_HH_GUARD
#define WORKER_HH_GUARD

#include <thread>
#include <functional>
#include "Collector.hh"
#include "Scheduler.hh"
#include "Timer.hh"
#include <mutex>

namespace spm {
  std::mutex write_to_output_stream;

  template <class InputType, class OutputType>
  class Worker {

  private:
    enum class State : short { LIVING, WILL_EVENTUALLY_TERMINATE };
    std::function<OutputType (InputType)> f;
    std::thread* t;
    int id;
    State state;
  public:

    Worker(std::function<OutputType (InputType)> f, int id)
      : f(f), id(id), t(nullptr), state(State::LIVING)
    { }

    Worker(const Worker&) = delete;
    Worker(const Worker&&) = delete;

    ~Worker() {
      delete t;
    }

    int get_id() {
      return id;
    }

    void exec(InputType x, Collector<OutputType>* collector,
              Scheduler<InputType, OutputType>* scheduler) {

      // std::cout << "Worker_" << id << "::exec()" << std::endl;

      t = new std::thread([x, this, collector, scheduler] {
                            // Timer t("Step (" + std::to_string(x) + ")")
                            InputType res = f(x);
                            {
                              std::unique_lock<std::mutex> lock(write_to_output_stream);
                              collector->push_back(res);
                            }
                            scheduler->done(this);
                          });
    }

    bool will_terminate() {
      return state == State::WILL_EVENTUALLY_TERMINATE;
    }

    void drop() {
      state = State::WILL_EVENTUALLY_TERMINATE;
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
