#ifndef MONITOR_GUARD_HH
#define MONITOR_GUARD_HH

#include "Emitter.hh"
#include "Collector.hh"

class Monitor {

public:

  Monitor(Collector* collector,
          Emitter* scheduler,
          float ts_goal,
          int input_stream_len)
    : _collector(collector),
      _scheduler(scheduler),
      stop(false),
      input_stream_len(input_stream_len),
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

                            auto no_results = _collector->results.size();

                            if (no_results == 0) {
                              //start monitoring when the output vector starts filling up.
                              continue;
                            }

                            if (_scheduler->data_size() == 0) {
                              std::clog << "Monitor::WAITING" << std::endl;
                              std::clog << "NO_RESULTS = " << no_results << "/" << input_stream_len << std::endl;
                              continue;
                            }

                            if (input_stream_len == no_results) {
                              continue;
                            }

                            default_policy(no_results - this->prev_no_results, no_results);
                            this->prev_no_results = no_results;
                          }
                        });
  }

  void join() {
    stop = true;
    t->join();
  }

private:
  Collector* _collector;
  Emitter* _scheduler;
  bool stop;
  float ts_goal;
  std::vector<float> times;
  unsigned prev_no_results;
  int input_stream_len;
  std::thread* t;

  static constexpr float RANGE[2] {0.85, 1.15};

  void default_policy(unsigned no_results, unsigned total_results) {
    float ts = (no_results > 0) ? 1.0/no_results : 2;
    float ratio = ts/ts_goal;

    times.push_back(ts);
    // std::cout << _scheduler->get_no_workers() << "," << ts << std::endl;
    // std::clog << "Ts = " << ts << " - " << _scheduler->get_no_workers();
    std::clog << " - NO_RESULTS = " << total_results << "/" << input_stream_len << std::endl;

    if (ratio <= RANGE[0]) {
      unsigned to_remove = std::sqrt((int)(1/ratio));

      std::clog << "Monitor::REMOVE_" << to_remove;
      std::clog << "_WORKER" << (to_remove > 1 ? "S" : "");
      std::clog << std::endl;

      _scheduler->remove_workers(to_remove);
      return;
    }

    if (ratio >= RANGE[1]) {
      unsigned to_add = std::sqrt((int)ratio);

      std::clog << "Monitor::ADD_" << to_add;
      std::clog << "_WORKER" << (to_add > 1 ? "S" : "");
      std::clog << std::endl;

      _scheduler->add_workers(to_add);
      return;
    }

    std::clog << "Monitor::DO_NOTHING" << std::endl;
  }
};

#endif
