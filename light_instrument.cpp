#include "light_instrument.h"

LightInstrument::LightInstrument(NPin pin)
  : pin(pin) {}

void LightInstrument::setup() {
  pinMode(pin, OUTPUT);
  reset();
}

void LightInstrument::tick(Time time) {}

void LightInstrument::stop() {
  digitalWrite(pin, LOW);
  is_lighting = false;
  current_pitch = Note::NULL_PITCH;
}

void LightInstrument::reset() {
  stop();
}

void LightInstrument::note_on(Note::NPitch pitch, Velocity velocity) {
  if (velocity == 0) {
    return note_off(pitch, velocity);
  }

  current_pitch = pitch;
  digitalWrite(pin, HIGH);
  is_lighting = true;
}

void LightInstrument::note_off(Note::NPitch pitch, Velocity velocity) {
  if (pitch == current_pitch) {
    stop();
  }
}

Note::NPitch LightInstrument::get_current_pitch() const {
  return current_pitch;
}
