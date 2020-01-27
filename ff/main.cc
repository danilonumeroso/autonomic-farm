#include <thread>
#include <set>
#include <math.h>
#include <map>
#include <chrono>
#include <iostream>
#include <functional>
#include <ff/ff.hpp>
#include <random>

#include "../src/Timer.hh"
#include "Emitter.hh"
#include "Worker.hh"
#include "Collector.hh"
#include "Monitor.hh"

using namespace ff;

const int MAX_NW = 128;

int program(int time) {
  std::this_thread::sleep_for(std::chrono::milliseconds(time));
  return time;
}

// External Emitter
struct FirstStage: ff_node_t<int> {

  FirstStage(std::vector<int>* stream)
    : stream(stream)
  { }

  int* svc(int*) {
    for (auto first = stream->rbegin(), last = stream->rend(); first != last; ++first) {
      ff_send_out(new int(*first));
    }
    return EOS;
  }

  std::vector<int>* stream;
};

const int L1 = 100;
const int L4 = 400;
const int L8 = 800;
const int NO_ELEMENTS = 200;
const float STD_DEV = 0;
#ifdef TEST_4L_1L_8L

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();
  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L4, std::min<int>(L4/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1,d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L1, std::min<int>(L1/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L8, std::min<int>(L8/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  return stream;
}

#endif

#ifdef TEST_1L_4L_8L


std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();
  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L1, std::min<int>(L1/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1,d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L4, std::min<int>(L4/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L8, std::min<int>(L8/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  return stream;
}

#endif

#ifdef TEST_8L_4L_1L

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();
  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L8, std::min<int>(L8/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1,d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L4, std::min<int>(L4/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  {
    std::default_random_engine g;
    std::normal_distribution<float> d((float)L1, std::min<int>(L1/2, STD_DEV));
    for (int i = 0; i < NO_ELEMENTS; ++i) {
      stream->push_back(std::max<int>(1, d(g)));
    }
  }

  return stream;
}

#endif

#ifdef CONSTANT_TEST

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();

  for (int i = 0; i < NO_ELEMENTS * 3; ++i) {
    stream->push_back(L4+L1);
  }

  return stream;
}

#endif

int main(int argc, char* argv[]) {
  if(argc < 3){
    std::cout << "Usage: ./ff [n_workers] [ts_goal]" << std::endl;
    return 1;
  }

  int nw = atoi(argv[1]);
  float ts_goal = atof(argv[2]);

  FirstStage fs(get_sample_stream());

  // for (const auto& i: *(fs.stream)) {
  //   std::cout << i << std::endl;
  // }

  std::vector<ff_node*> workers;

  ff_farm ff_autonomic_farm;

  for (auto i=0; i < MAX_NW; ++i) {
    workers.push_back(new Worker(program));
  }

  ff_autonomic_farm.add_workers(std::move(workers));
  ff_autonomic_farm.cleanup_workers();

  Emitter E(ff_autonomic_farm.getlb(), nw);
  ff_autonomic_farm.remove_collector();
  ff_autonomic_farm.add_emitter(&E);
  ff_autonomic_farm.wrap_around(); // connects workers to emitter

  Collector C;
  ff_autonomic_farm.add_collector(&C);
  ff_autonomic_farm.wrap_around(); // connects collector to emitter


  ff_Pipe<> pipe(fs, ff_autonomic_farm);
  {
    Timer<std::chrono::milliseconds> t("FF Version");
    Monitor monitor(&C, &E, ts_goal, fs.stream->size());
    monitor.execute();
    if (pipe.run_then_freeze()<0) {
      error("running pipe\n");
      return -1;
    }
    pipe.wait_freezing();
    monitor.join();
    pipe.wait();
  }

  // for (const auto& r: C.results) {
  //   std::cout << *r << std::endl;
  // }
  return 0;
}
