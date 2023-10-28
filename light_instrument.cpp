#include "light_instrument.h"

LightInstrument::LightInstrument(uint8_t pin)
  : pin(pin) {}

void LightInstrument::setup() {
  pinMode(pin, OUTPUT);
  reset();
}

void LightInstrument::tick(Time time) {}

void LightInstrument::stop() {
  digitalWrite(pin, LOW);
  is_lighting = false;
  current_note = nullptr;
}

void LightInstrument::reset() {
  stop();
}

void LightInstrument::note_on(const Note & note, uint8_t velocity) {
  if (velocity == 0) {
    return note_off(note, velocity);
  }

  current_note = &note;
  digitalWrite(pin, HIGH);
  is_lighting = true;
}

void LightInstrument::note_off(const Note & note, uint8_t velocity) {
  if (&note != current_note) {
    return;
  }
  stop();
}
