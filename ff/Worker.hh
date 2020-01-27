#ifndef WORKER_GUARD_HH
#define WORKER_GUARD_HH

#include <mutex>
#include <functional>
#include <ff/ff.hpp>

using namespace ff;

std::mutex out_stream;

struct Worker: ff_monode_t<int> {

  typedef std::function<int(int)> Task;

  Worker(Task t)
    : task(t)
  { }

  int* svc(int* in) {
    {
      // std::unique_lock<std::mutex> lock(out_stream);
      // std::cout << "Worker(" << get_my_id() << ")::svc()" << std::endl;
    }
    int* out = new int(task(*in));
    ff_send_out_to(out, 0);
    ff_send_out_to(out, 1);
    return GO_ON;
  }

  void eosnotify(ssize_t) {
    // std::unique_lock<std::mutex> lock(out_stream);
    // std::cout << "Worker(" << get_my_id() << ")::bye" << std::endl;
  }


  void svc_end() {
    // std::unique_lock<std::mutex> lock(out_stream);
    // std::cout << "Worker(" << get_my_id() << ")::sleep" << std::endl;
  }

  Task task;
};

#endif
