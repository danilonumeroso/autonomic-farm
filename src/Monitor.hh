#ifndef MONITOR_HH_GUARD
#define MONITOR_HH_GUARD

#include "Collector.hh"
#include "Scheduler.hh"
#include <algorithm>

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
        prev_no_results(0U),
        times()
    { }

    ~Monitor() {
      delete t;
    }

    void execute() {
      t = new std::thread([this]{
                            while (!this->stop) {
                              std::this_thread::sleep_for(std::chrono::milliseconds(500));

                              auto no_results = this->_collector->no_results();

                              if (no_results == 0) {
                                //start monitoring when the output vector starts filling up.
                                continue;
                              }

                              default_policy(no_results - this->prev_no_results);
                            }
                          });
    }

    void join() {
      stop = true;
      t->join();
    }

  private:
    Collector<OutputType>* _collector;
    Scheduler<InputType, OutputType>* _scheduler;
    bool stop;
    float ts_goal;
    std::vector<float> times;
    unsigned prev_no_results;
    std::thread* t;

    void default_policy(unsigned no_results) {
      float ts = 0.5/no_results;
      float ratio = ts/ts_goal;

      times.push_back(ts);

      std::cout << "Monitor:: measured service time: " << ts << std::endl;

      if (ratio <= 0.8) {
        _scheduler->remove_worker();
        std::cout << "Attempt to remove worker" << std::endl;
        return;
      }

      if (ratio >= 1.2) {
        unsigned to_add = std::min((int) ratio, 1);
        _scheduler->add_worker(to_add);
        std::cout << "Attempt to add " << to_add << " workers" << std::endl;
        return;
      }
    }
  };
}
#endif
