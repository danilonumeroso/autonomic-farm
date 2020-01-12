#ifndef COLLECTOR_HH_GUARD
#define COLLECTOR_HH_GUARD

template <class OutputType>
class Collector {
public:

  void push_back(const OutputType& r) {
    results.push_back(r);
  }

  const std::vector<OutputType>& get_results() const {
    return results;
  }

private:
  std::vector<OutputType> results;
};

#endif
