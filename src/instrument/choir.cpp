#include "choir.h"

using instrument::Choir;

Choir::Choir(NInstrument n_instruments, IInstrument ** instruments)
: n_instruments(n_instruments), instruments(instruments) {}

void Choir::setup() {
  for (NInstrument i = 0; i < n_instruments; i++) {
    instruments[i]->setup();
  }
}

void Choir::tick(Time time) {
  for (NInstrument i = 0; i < n_instruments; i++) {
    instruments[i]->tick(time);
  }
}

void Choir::stop() {
  for (NInstrument i = 0; i < n_instruments; i++) {
    instruments[i]->stop();
  }
  current_pitch = Note::NULL_PITCH;
}

void Choir::reset() {
  for (NInstrument i = 0; i < n_instruments; i++) {
    instruments[i]->reset();
  }
}

void Choir::note_on(Note::NPitch pitch, Velocity velocity) {
  if (velocity == 0) {
    return note_off(pitch, velocity);
  }

  for (NInstrument i = 0; i < n_instruments; i++) {
    instruments[i]->note_on(pitch, velocity);
  }
  current_pitch = pitch;
}

void Choir::note_off(Note::NPitch pitch, Velocity velocity) {
  if (pitch == current_pitch) {
    stop();
  }
}

Note::NPitch Choir::get_current_pitch() const {
  return current_pitch;
}
