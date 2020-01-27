#ifndef COLLECTOR_GUARD_HH
#define COLLECTOR_GUARD_HH

#include <vector>

using namespace ff;

struct Collector: ff_minode_t<int> {
  int* svc(int* in) {
    results.push_back(in);
    return GO_ON;
  }

  std::vector<int*> results;
};

#endif
