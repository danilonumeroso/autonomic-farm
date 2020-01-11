#include "Farm.hh"
#include <iostream>
#include <thread>
#include "Timer.hh"

int fatt(int x) {
  if (x == 0) {
    return 1;
  }
  return x * fatt(x-1);
}

int program(int x) {
  int result = 0;

  for (int i=0; i<x*100; ++i) {
    result += fatt(50);
  }

  return result;
}

void test1() {
  Farm<int,int>({1,2,3,4,5}, program,5);

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
