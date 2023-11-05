#include "queue.h"

using instruments_group::Queue;

Queue::Queue(NInstrument n_instruments, IInstrument ** instruments)
: n_instruments(n_instruments), instruments(instruments),
  front_index(0), back_index(-1), current_size(n_instruments) {}

NInstrument Queue::size() const {
  return current_size;
}

bool Queue::is_empty() const {
  return current_size == 0;
}

bool Queue::is_full() const {
  return current_size == n_instruments;
}

IInstrument * Queue::pop() {
  if (current_size == 0) {
    return nullptr;
  }

  IInstrument * instrument = instruments[front_index];
  front_index = (front_index + 1) % n_instruments;
  current_size--;

  return instrument;
}

void Queue::push(IInstrument * instrument) {
  if (current_size >= n_instruments) {
    return;
  }

  back_index = (back_index + 1) % n_instruments;
  instruments[back_index] = instrument;
  current_size++;
}
