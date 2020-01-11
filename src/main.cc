#include "Farm.hh"
#include <iostream>
#include <thread>
#include "Timer.hh"

int program(int x) {
  int result = 0;

  std::this_thread::sleep_for(std::chrono::microseconds(x));

  return result;
}

void test1() {
  Farm<int,int>({500, 100, 800}, program, 1);
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
  seq();
}
