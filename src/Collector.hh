#ifndef COLLECTOR_HH_GUARD
#define COLLECTOR_HH_GUARD

template <class OutputType>
class Collector {
private:
  std::vector<OutputType> results;

public:
  void push_back(const OutputType& r) {
    results.push_back(r);
  }

  const std::vector<OutputType>& get_results() {
    return results;
  }
};

#endif
