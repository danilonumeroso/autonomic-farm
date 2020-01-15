#include "../src/Farm.hh"
#include <iostream>
#include <thread>
#include "../src/Timer.hh"

#ifdef TEST_4L_1L_8L

std::initializer_list<int> l = {300,400,500,400,700,100,600,200, 400, 500, 300, 200, 600,     //4L
                                100,50,150,100,100,50,150,100, 50, 300, 50, 50, 50,           //1L
                                800,800,800,800,200,1400,1000,600, 100, 1500, 50, 1550, 800}; //8L
#endif

#ifdef TEST_1L_4L_8L

std::initializer_list<int> l = {100,50,150,100,100,50,150,100, 50, 300, 50, 50, 50,     //1L
                                300,400,500,400,700,100,600,200, 400, 500, 300, 200, 600, //4L
                                800,800,800,800,200,1400,1000,600, 100, 1500, 50, 1550, 800}; //8L
#endif

#ifdef TEST_8L_4L_1L

std::initializer_list<int> l = {800,800,800,800,200,1400,1000,600, 100, 1500, 50, 1550, 800,  //8L
                                300,400,500,400,700,100,600,200, 400, 500, 300, 200, 600, //4L
                                100,50,150,100,100,50,150,100, 50, 300, 50, 50, 50,};      //1L
#endif


#ifdef TEST_8L_1L_4L

std::initializer_list<int> l = {800,800,800,800,200,1400,1000,600, 100, 1500, 50, 1550, 800,  //8L
                                100,50,150,100,100,50,150,100, 50, 300, 50, 50, 50,      //1L
                                300,400,500,400,700,100,600,200, 400, 500, 300, 200, 600 };    //4L
#endif

int program(int x) {
  std::this_thread::sleep_for(std::chrono::milliseconds(x));

  return x;
}

void par(int nw, float ts_goal) {
  spm::Farm<int,int> farm(l, program, ts_goal, nw);
  std::vector<int> const* results;
  {
    Timer j("Whole job");
    results = &farm.run();
  }

  std::cout << results->size() << " results collected" << std::endl;

  // for (auto i = results->cbegin(), end = results->cend(); i < end; ++i) {
  //   std::cout << *i << std::endl;
  // }
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
