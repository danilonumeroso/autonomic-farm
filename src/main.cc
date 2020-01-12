#include "Farm.hh"
#include <iostream>
#include <thread>
#include "Timer.hh"

int program(int x) {
  std::this_thread::sleep_for(std::chrono::microseconds(x));

  return x;
}

void par1() {
  Farm<int,int> farm({500, 100, 800, 400}, program, 3);
  std::vector<int> const* results;
  {
    Timer j("Whole job");
    results = &farm.run();
  }

  for (auto start = results->cbegin(), end = results->cend(); start < end; ++start) {
    std::cout << *start << std::endl;
  }
}

void seq() {
  std::vector<int> stream {500, 100, 800};
  {
    Timer s("Sequential version");
    for (auto start = stream.cbegin(), end = stream.cend(); start < end; ++start) {
      Timer s("Step");
      program(*start);
    }
  }
}


int main() {
  par1();
}
