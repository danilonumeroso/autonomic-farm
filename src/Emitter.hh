#ifndef EMITTER_HH_GUARD
#define EMITTER_HH_GUARD

#include <vector>

template <class InputType>
class Emitter {
private:
  std::vector<InputType> input_stream;
  unsigned i;

public:
  Emitter(std::vector<InputType>&& stream)
    : input_stream(stream),
      i(0)
  { }

  const InputType& operator()() {
    if (i >= input_stream.size()) {
      throw "Reached end of stream";
    }

    return input_stream[i++];
  }

};

#endif
