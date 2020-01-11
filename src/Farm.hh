#ifndef FARM_HH_GUARD
#define FARM_HH_GUARD

#include "Emitter.hh"
#include "Collector.hh"
#include "Worker.hh"
#include <functional>

template <class InputType, class OutputType>
class Farm {

public:
  typedef std::vector<Worker<InputType, OutputType>> Workers;

  Farm(std::initializer_list<InputType> stream,
       std::function<OutputType (InputType)> f,
       unsigned nw) : _emitter(stream),
                      _collector(),
                      _worker(f),
                      _nw(nw)
  { }

private:
  Emitter<InputType> _emitter;
  Collector<OutputType> _collector;
  Worker<InputType, OutputType> _worker;
  unsigned _nw;
  // Scheduler
  // Monitor
};

#endif
