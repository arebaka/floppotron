#include "stack.h"

using instruments_group::Stack;

Stack::Stack(NInstrument n_instruments, IInstrument ** instruments)
: n_instruments(n_instruments), instruments(instruments), current_size(n_instruments) {}

NInstrument Stack::size() const {
  return current_size;
}

bool Stack::is_empty() const {
  return current_size == 0;
}

bool Stack::is_full() const {
  return current_size == n_instruments;
}

IInstrument * Stack::pop() {
  if (current_size == 0) {
    return nullptr;
  }

  IInstrument * instrument = instruments[current_size - 1];
  current_size--;

  return instrument;
}

void Stack::push(IInstrument * instrument) {
  if (current_size >= n_instruments) {
    return;
  }

  instruments[current_size] = instrument;
  current_size++;
}
