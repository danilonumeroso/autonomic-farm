#include "../src/Farm.hh"
#include <iostream>
#include <thread>
#include "../src/Timer.hh"

int program(int x) {
  std::this_thread::sleep_for(std::chrono::microseconds(x));

  return x;
}

void par(int nw, float ts_goal) {
  spm::Farm<int,int> farm({500, 100, 800, 400}, program, nw);
  {
    Timer j("Whole job");
    farm.run();
  }
}

void seq() {
  std::vector<int> stream {500, 100, 800, 400};
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
