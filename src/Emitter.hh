#ifndef EMITTER_HH_GUARD
#define EMITTER_HH_GUARD

#include <vector>

namespace spm {

  template <class InputType>
  class Emitter {
  private:
    std::vector<InputType> input_stream;
    unsigned i;

  public:

    Emitter(Emitter&) = delete;
    Emitter(Emitter&&) = delete;

    Emitter(std::vector<InputType>&& stream)
      : input_stream(stream),
        i(0)
    { }

    const InputType& emit() {
      if (is_empty()) {
        throw "Reached end of stream";
      }

      return input_stream[i++];
    }

    bool is_empty() {
      return i >= input_stream.size();
    }

  };
}
#endif
