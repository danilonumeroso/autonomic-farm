#include "../src/Farm.hh"
#include <iostream>
#include <thread>
#include "../src/Timer.hh"
#include "../src/Constant.hh"

const int L1 = 100;
const int L4 = 400;
const int L8 = 800;
const int NO_ELEMENTS = 200;

#ifdef TEST_4L_1L_8L

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L4);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L1);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L8);
  }

  return stream;
}

#endif

#ifdef TEST_1L_4L_8L

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L1);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L4);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L8);
  }

  return stream;
}

#endif

#ifdef TEST_8L_4L_1L

std::vector<int>* get_sample_stream() {
  std::vector<int>* stream = new std::vector<int>();

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L8);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L4);
  }

  for (int i = 0; i < NO_ELEMENTS; ++i) {
    stream->push_back(L1);
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

  // for (auto i = results->cbegin(), end = results->cend(); i < end; ++i) {
  //   std::cout << *i << std::endl;
  // }
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
