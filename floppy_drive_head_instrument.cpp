#include "floppy_drive_head_instrument.h"

const uint16_t FloppyDriveHeadInstrument::DEFAULT_N_POSITIONS = 80;  // for general 3.5 and 5.25 720K+
const uint16_t FloppyDriveHeadInstrument::TIME_TO_WAIT_AFTER_RESET = 100;

void FloppyDriveHeadInstrument::reverse() {
  direction = direction == HIGH ? LOW : HIGH;
  digitalWrite(direction_pin, direction);
}

void FloppyDriveHeadInstrument::toggle_stepping() {
  // reverse if end has been reached
  int16_t steps_left = direction == HIGH ? n_positions - position - 1 : position;
  if (steps_left <= 0) {
    reverse();
  }

  if (is_stepping) {
    note_steps_counter++;
    if (direction == HIGH) {
      position++;
    } else {
      position--;
    }
  }

  digitalWrite(step_pin, is_stepping ? LOW : HIGH);
  is_stepping = !is_stepping;
}

FloppyDriveHeadInstrument::FloppyDriveHeadInstrument(uint8_t step_pin, uint8_t direction_pin, uint8_t n_positions)
  : step_pin(step_pin), direction_pin(direction_pin), n_positions(n_positions) {}

void FloppyDriveHeadInstrument::setup() {
  pinMode(direction_pin, OUTPUT);
  pinMode(step_pin, OUTPUT);
  reset();
  delay(TIME_TO_WAIT_AFTER_RESET);  // wait for safety
}

void FloppyDriveHeadInstrument::tick(Time time) {
  if (current_note == nullptr) {
    return;
  }

  inactive_time += time;
  if (inactive_time >= current_halfperiod) {
    toggle_stepping();
    inactive_time %= current_halfperiod;
  }
}

void FloppyDriveHeadInstrument::stop() {
  digitalWrite(step_pin, LOW);
  is_stepping = false;
  current_note = nullptr;
  current_halfperiod = 0;
  inactive_time = 0;
  note_steps_counter = 0;
}

void FloppyDriveHeadInstrument::reset() {
  stop();
  digitalWrite(direction_pin, LOW);
  direction = LOW;

  for (uint16_t i = 0; i < n_positions; i++) {
    digitalWrite(step_pin, HIGH);
    delay(1);
    digitalWrite(step_pin, LOW);
    delay(1);
  }

  position = 0;
}

void FloppyDriveHeadInstrument::note_on(const Note & note, uint8_t velocity) {
  if (velocity == 0) {
    return note_off(note, velocity);
  }
  current_note = &note;
  current_halfperiod = note.period / 2;
}

void FloppyDriveHeadInstrument::note_off(const Note & note, uint8_t velocity) {
  if (&note != current_note) {
    return;
  }
  uint32_t steps_counter = note_steps_counter;  // save the value
  stop();

  // reverse to direction with more available steps to hold a next note without reversion if it is the same in duration
  int16_t steps_left = direction == HIGH ? n_positions - position - 1 : position;
  if (steps_left < steps_counter && n_positions - steps_left > steps_counter) {
    reverse();
  }
}

const Note * FloppyDriveHeadInstrument::get_current_note() const {
  return current_note;
}
