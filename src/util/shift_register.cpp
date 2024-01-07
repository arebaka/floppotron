#include "Arduino.h"
#include "HardwareSerial.h"
#include "shift_register.h"

using util::ShiftRegister;

void ShiftRegister::set_state_bit(NBit bit, Value value) {
  state = (state & ~1 << bit) | value << bit;
}

ShiftRegister::ShiftRegister(NPin data_pin, NPin clock_pin, NPin latch_pin)
: data_pin(data_pin), clock_pin(clock_pin), latch_pin(latch_pin),
  state(0), n_instruments(0), n_ticked_instruments(0)
{
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
}

void ShiftRegister::attach_instrument(IInstrument * instrument) {
  if (n_instruments >= 8) {
    return;
  }
  instruments[n_instruments] = instrument;
  n_instruments++;
}

void ShiftRegister::write(IInstrument * instrument, NPin pin, Value value) {
  set_state_bit(pin, value);
}

void ShiftRegister::tick(IInstrument * instrument) {
  n_ticked_instruments++;

  if (n_ticked_instruments >= n_instruments) {
    sync();
    n_ticked_instruments = 0;
  }
}

void ShiftRegister::sync() {
  digitalWrite(latch_pin, LOW);
  shiftOut(data_pin, clock_pin, MSBFIRST, state);
  digitalWrite(latch_pin, HIGH);
}
