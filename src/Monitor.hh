#ifndef MONITOR_HH_GUARD
#define MONITOR_HH_GUARD

#include "Collector.hh"
#include "Scheduler.hh"
#include "Constant.hh"
#include <algorithm>
#include <cmath>

namespace spm {

  template <class InputType, class OutputType>
  class Monitor {

  public:
    Monitor(Monitor&) = delete;
    Monitor(Monitor&&) = delete;

    explicit Monitor(Collector<OutputType>* collector,
                     Scheduler<InputType, OutputType>* scheduler,
                     float ts_goal)
      : _collector(collector),
        _scheduler(scheduler),
        stop(false),
        ts_goal(ts_goal),
        times(),
        prev_no_results(0U)
    { }

    ~Monitor() {
      delete t;
    }

    void execute() {
      t = new std::thread([this]{
                            while (!this->stop) {
                              std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                              auto no_results = this->_collector->no_results();

                              if (no_results == 0) {
                                //start monitoring when the output vector starts filling up.
                                continue;
                              }

                              default_policy(no_results - this->prev_no_results);
                              this->prev_no_results = no_results;
                            }
                          });
    }

    void join() {
      stop = true;
      t->join();
    }

  private:
    enum class State { DECREASE = -1, NOTHING, INCREASE };

    Collector<OutputType>* _collector;
    Scheduler<InputType, OutputType>* _scheduler;
    bool stop;
    float ts_goal;
    std::vector<float> times;
    unsigned prev_no_results;
    std::thread* t;
    State action = State::NOTHING;

    static constexpr float RANGE[2] {0.9, 1.1};

    void default_policy(unsigned no_results) {
      float ts = (no_results > 0) ? 1.0/no_results : 2;
      float ratio = ts/ts_goal;

      times.push_back(ts);
#ifdef ENABLE_REPORT
      REPORT(std::to_string(_scheduler->get_no_active_workers())
             .append(",")
             .append(std::to_string(ts)));
#endif
      LOG(std::string("Ts=")
          .append(std::to_string(ts))
          .append(" - ")
          .append(std::to_string(_scheduler->get_no_active_workers()))
          );

      if (ratio <= RANGE[0]) {
        State prev = action;
        action = State::DECREASE;

        if (prev == State::INCREASE) {
          LOG(std::string("Monitor::STABILITY"));
          return;
        }

        unsigned to_remove = std::sqrt((int)(1/ratio));
        // unsigned to_remove = (int)(1/ratio);
        LOG(std::string("Monitor::REMOVE_")
            .append(std::to_string(to_remove))
            .append("_WORKER")
            .append(to_remove > 1 ? "S" : ""));
        _scheduler->remove_worker(to_remove);
        return;
      }

      if (ratio >= RANGE[1]) {
        State prev = action;
        action = State::INCREASE;

        if (prev == State::DECREASE) {
          LOG(std::string("Monitor::STABILITY"));
          return;
        }

        unsigned to_add = std::sqrt((int)ratio);
        // unsigned to_add = (int)ratio;
        LOG(std::string("Monitor::ADD_")
            .append(std::to_string(to_add))
            .append("_WORKER")
            .append(to_add > 1 ? "S" : ""));
        _scheduler->add_worker(to_add);
        return;
      }

      action = State::NOTHING;
      LOG("Monitor::DO_NOTHING");
    }
  };
}
#endif
