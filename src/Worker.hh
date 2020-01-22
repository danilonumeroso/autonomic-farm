#ifndef WORKER_HH_GUARD
#define WORKER_HH_GUARD

#include <thread>
#include <functional>
#include "Collector.hh"
#include "Scheduler.hh"
#include "Timer.hh"
#include <mutex>

namespace spm {

  template <class InputType, class OutputType>
  class Worker {
  public:

    typedef Collector<OutputType> CollectorType;
    typedef Scheduler<InputType, OutputType> SchedulerType;

    typedef std::function<OutputType(InputType)> Task;
    typedef unsigned Integer;

    enum class State : short { LIVING, WILL_EVENTUALLY_TERMINATE };

    Worker() = default;

    Worker(Task t, Integer id)
      : f(t), id(id), state(State::LIVING)
    { }

    inline Integer get_id() {
      return id;
    }

    inline bool will_terminate() {
      return state == State::WILL_EVENTUALLY_TERMINATE;
    }

    inline void terminate() {
      state = State::WILL_EVENTUALLY_TERMINATE;
    }

    inline void revive() {
      state = State::LIVING;
    }

    virtual void run(InputType x, CollectorType* c, SchedulerType* s) = 0;
    virtual void join() = 0;

  protected:
    Task f;
    Integer id;
    State state;
  };

  template <class InputType, class OutputType>
  class SpmWorker : public Worker<InputType, OutputType> {

  private:
    using Base = Worker<InputType, OutputType>;
    std::thread* t;
  public:
    typedef typename Base::CollectorType CollectorType;
    typedef typename Base::SchedulerType SchedulerType;
    typedef typename Base::Task Task;
    typedef typename Base::Integer Integer;

    SpmWorker(Task task, Integer id) : Base(task, id), t(nullptr)
    { }

    ~SpmWorker() {
      delete t;
    }

    virtual void run(InputType x, CollectorType* c, SchedulerType* s) {
      t = new std::thread([x, this, c, s] {
                            InputType res = Base::f(x);
                            c->push_back(res);
                            s->done(this);
                          });
    }

    virtual void join() {
      if (t == nullptr) {
        return;
      }
      t->join();
    }
  };
}
#endif
