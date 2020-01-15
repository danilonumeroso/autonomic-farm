#include "../src/Farm.hh"
#include <iostream>
#include <thread>
#include "../src/Timer.hh"

std::initializer_list<int> l = {500, 100, 800, 400, 300, 500, 800, 1000, 200, 100, 100, 100};

int program(int x) {
  std::this_thread::sleep_for(std::chrono::milliseconds(x));

  return x;
}

void par(int nw, float ts_goal) {
  spm::Farm<int,int> farm(l, program, ts_goal, nw);
  {
    Timer j("Whole job");
    farm.run();
  }
}

void seq() {
  std::vector<int> stream(l);
  {
    Timer s("Sequential version");
    for (auto start = stream.cbegin(), end = stream.cend(); start < end; ++start) {
      Timer s("Step");
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
