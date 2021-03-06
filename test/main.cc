#include "../src/Farm.hh"
#include <iostream>
#include <thread>
#include "../src/Timer.hh"
#include "../src/Constant.hh"
#include <iterator>
#include <random>

const int L1 = 100;
const int L4 = 400;
const int L8 = 800;
const int NO_ELEMENTS = 200;
const float STD_DEV = 200.0;
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

int program(int x) {
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
  return x;
}

void par(int nw, float ts_goal) {
  spm::Farm<int,int> farm(std::move(*get_sample_stream()), program, ts_goal, nw);
  std::vector<int> const* results;
  {
    Timer<std::chrono::milliseconds> j("Whole job");
    results = &farm.run();
  }

  std::clog << results->size() << " results collected" << std::endl;
}

void seq() {
  std::vector<int>* stream = get_sample_stream();
  {
    Timer<std::chrono::seconds> s("Sequential version");
    for (auto start = stream->cbegin(), end = stream->cend(); start < end; ++start) {
      Timer<std::chrono::milliseconds> s("Step");
      program(*start);
    }
  }
}


int main(int argc, char** argv) {
  if (argc == 1) {
    seq();
    return 0;
  }

  if (argc == 3) {
    par(atoi(argv[1]),
        atof(argv[2]));
    return 0;
  }

  std::cout << "Usage: [name] [nw] [ts_goal]" << std::endl;
  return 1;
}
