#ifndef EMITTER_GUARD_HH
#define EMITTER_GUARD_HH
#include <atomic>
#include <ff/ff.hpp>
#include <deque>
#include "Synchronization.hh"

using namespace ff;

extern const int MAX_NW;

class Emitter: public ff_monode_t<int> {
public:
  Emitter(ff_loadbalancer* lb, int nw)
    : load_balancer(lb), data(), nw(nw),
      ready(), nready(), eos_received(false),
      done(false)
  { }

  void add_workers(int n) {

    if (n >= reservoir.size()) {
      n = reservoir.size() - 1;
    }
    __LOCK_WHILE_SCHEDULING__

    for (int i=0; i<n; ++i) {
      int wid = reservoir.front();
      reservoir.pop_front();

      lb->thaw(wid, true);
      ready.push_back(true);
      ++nready;
    }

    while (data.size() > 0 && nready.load() > 0) {
      int worker = select_worker();
      lb->ff_send_out_to(data.back(), worker);
      std::cout << "assign_to(" << worker << ")" << std::endl;
      data.pop_back();
      ready[worker] = false;
      --nready;
    }
  }

  int data_size() {
    return data.size();
  }

  void remove_workers(int n) {

    if (n >= ready.size()) {
      n = ready.size() - 1;
    }

    std::vector<int> to_be_freezed;

    __LOCK_WHILE_SCHEDULING__

    for (auto i=0; i<n; ++i) {
      int wid = ready.size() - (1+i);
      to_be_freezed.push_back(wid);
      ff_send_out_to(GO_OUT, wid);
    }

    for (int wid: to_be_freezed) {
      lb->wait_freezing(wid);

      if (ready[wid]) {
        --nready;
      }
      ready.pop_back();
      std::cout << "remove(" << wid << ")" << std::endl;
      reservoir.push_front(wid);

    }
  }

  int svc_init() {
    last = nw;
    ready.resize(nw);

    for (auto i=0; i < ready.size(); ++i) {
      ready[i] = true;
    }

    nready.store(ready.size());

    for(int i = nw; i < MAX_NW; ++i) {
      ff_send_out_to(GO_OUT, i);
    }

    for(int i = nw; i < MAX_NW; ++i){
      lb->wait_freezing(i);
      this->reservoir.push_back(i);
    }

    return 0;
  }

  int* svc(int* in) {
    int wid = lb->get_channel_id();
    if (wid == -1) {
      int worker;

      if ((worker = select_worker()) == -1) {
        data.push_back(in);
        return GO_ON;
      }

      lb->ff_send_out_to(in, worker);
      ready[worker] = false;
      --nready;
      return GO_ON;
    }
    __LOCK_WHILE_SCHEDULING__
    if (wid >= ready.size()) {
      std::clog << wid << " dead" << std::endl;
      return GO_ON;
    }

    ready[wid] = true;
    ++nready;
    if (data.size() > 0) {
      lb->ff_send_out_to(data.back(), wid);
      data.pop_back();
      ready[wid] = false;
      --nready;
    } else if (eos_received && nready.load() == ready.size()) {
      done = true;
      awake_all();
      lb->broadcast_task(EOS);
      return EOS;
    }

    return GO_ON;
  }

  void awake_all() {
    while (!reservoir.empty()) {
      lb->thaw(reservoir.front(), true);
      reservoir.pop_front();
    }
  }

  void eosnotify(ssize_t id) {
    if (id == -1) {
      // we have to receive all EOS from the previous stage
      // EOS is coming from the input channel

      eos_received=true;
      if (eos_received              &&
          nready.load() == ready.size()    &&
          data.size() == 0) {
        lb->broadcast_task(EOS);
      }
    }
  }

  int get_no_workers() {
    return ready.size();
  }

private:
  ff_loadbalancer* load_balancer;
  std::vector<int*> data;
  std::vector<bool> ready;
  std::deque<int> reservoir;
  bool eos_received;
  bool done;
  std::atomic<int> nready;
  int last;
  int nw;

  int select_worker() {
    for (unsigned i=last+1;i<ready.size();++i) {
      if (ready[i]) {
        last = i;
        return i;
      }
    }
    for (unsigned i=0;i<=last;++i) {
      if (ready[i]) {
        last = i;
        return i;
      }
    }
    return -1;
  }
};

#endif
