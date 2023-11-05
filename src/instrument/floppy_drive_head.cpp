#include "floppy_drive_head.h"

using instrument::FloppyDriveHead;

const FloppyDriveHead::NPosition FloppyDriveHead::DEFAULT_N_POSITIONS = 80;  // for general 3.5 and 5.25 720K+
const Time FloppyDriveHead::TIME_TO_WAIT_AFTER_SETUP = 100000;

void FloppyDriveHead::reverse() {
  direction = direction == HIGH ? LOW : HIGH;
  digitalWrite(direction_pin, direction);
}

void FloppyDriveHead::toggle_phase() {
  // reverse if end has been reached
  NStep steps_left = direction == HIGH ? n_positions - position - 1 : position;
  if (steps_left <= 1) {  // leave an extra step to avoid rattling when bumped
    reverse();
  }

  phase = phase == HIGH ? LOW : HIGH;
  digitalWrite(step_pin, phase);

  if (phase == HIGH) {
    note_steps_counter++;
    if (direction == HIGH) {
      position++;
    } else {
      position--;
    }
  }
}

FloppyDriveHead::FloppyDriveHead(NPin step_pin, NPin direction_pin, NPosition n_positions)
: step_pin(step_pin), direction_pin(direction_pin), n_positions(n_positions) {}

void FloppyDriveHead::setup() {
  pinMode(direction_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  reset();
  delay(TIME_TO_WAIT_AFTER_SETUP / 1000);  // wait for safety
}

void FloppyDriveHead::tick(Time time) {
  if (current_pitch == Note::NULL_PITCH) {
    return;
  }

  inactive_time += time;
  if (inactive_time >= current_halfperiod) {
    toggle_phase();
    inactive_time %= current_halfperiod;
  }
}

void FloppyDriveHead::stop() {
  digitalWrite(step_pin, LOW);
  phase = LOW;
  current_pitch = Note::NULL_PITCH;
  current_halfperiod = 0;
  inactive_time = 0;
  note_steps_counter = 0;
}

void FloppyDriveHead::reset() {
  stop();

  digitalWrite(direction_pin, LOW);
  for (NPosition i = 0; i < n_positions; i++) {
    digitalWrite(step_pin, HIGH);
    delay(1);
    digitalWrite(step_pin, LOW);
    delay(1);
  }

  digitalWrite(direction_pin, HIGH);
  direction = HIGH;
  position = 0;
}

void FloppyDriveHead::note_on(Note::NPitch pitch, Velocity velocity) {
  if (velocity == 0) {
    return note_off(pitch, velocity);
  }
  current_pitch = pitch;
  current_halfperiod = Note::pitches[pitch].period / 2;
}

void FloppyDriveHead::note_off(Note::NPitch pitch, Velocity velocity) {
  if (pitch != current_pitch) {
    return;
  }
  NStep steps_counter = note_steps_counter;  // save the value
  stop();

  // reverse to direction with more available steps to hold a next note without reversion if it is the same in duration
  NStep steps_left = direction == HIGH ? n_positions - position - 1 : position;
  if (steps_left < steps_counter && n_positions - steps_left > steps_counter) {
    reverse();
  }
}

Note::NPitch FloppyDriveHead::get_current_pitch() const {
  return current_pitch;
}
