#ifndef COLLECTOR_HH_GUARD
#define COLLECTOR_HH_GUARD

#include "Synchronization.hh"

namespace spm {

  template <class OutputType>
  class Collector {
  public:

    void push_back(const OutputType& r) {
      __LOCK_OUTPUT_STREAM__
      results.push_back(r);
    }

    const std::vector<OutputType>& get_results() const {
      return results;
    }

    unsigned no_results() const {
      return results.size();
    }

  private:
    std::vector<OutputType> results;
  };
}

#endif
