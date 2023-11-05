#include "light.h"

using instrument::Light;

Light::Light(NPin pin)
: pin(pin) {}

void Light::setup() {
  pinMode(pin, OUTPUT);
  reset();
}

void Light::tick(Time time) {}

void Light::stop() {
  digitalWrite(pin, LOW);
  is_lighting = false;
  current_pitch = Note::NULL_PITCH;
}

void Light::reset() {
  stop();
}

void Light::note_on(Note::NPitch pitch, Velocity velocity) {
  if (velocity == 0) {
    return note_off(pitch, velocity);
  }

  current_pitch = pitch;
  digitalWrite(pin, HIGH);
  is_lighting = true;
}

void Light::note_off(Note::NPitch pitch, Velocity velocity) {
  if (pitch == current_pitch) {
    stop();
  }
}

Note::NPitch Light::get_current_pitch() const {
  return current_pitch;
}
