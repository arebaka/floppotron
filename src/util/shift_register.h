#pragma once
#include "../instrument/i_instrument.h"

namespace util {
class ShiftRegister {
public:
  typedef uint8_t NBit;
  typedef uint8_t Value;

protected:
  const NPin data_pin;
  const NPin clock_pin;
  const NPin latch_pin;

  Value state;
  NInstrument n_instruments;
  NInstrument n_ticked_instruments;
  IInstrument * instruments[8];

  void set_state_bit(NBit bit, Value value);

public:
  ShiftRegister(NPin data_pin, NPin clock_pin, NPin latch_pin);

  void attach_instrument(IInstrument * instrument);
  void write(IInstrument * instrument, NPin pin, Value value);
  void tick(IInstrument * instrument);
  void sync();
};
}
