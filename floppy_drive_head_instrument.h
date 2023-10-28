#pragma once
#include "i_instrument.h"

class FloppyDriveHeadInstrument : public IInstrument {
public:
  static const uint16_t DEFAULT_N_POSITIONS;
  static const uint16_t TIME_TO_WAIT_AFTER_RESET;

protected:
  const uint8_t step_pin;
  const uint8_t direction_pin;
  const uint16_t n_positions;  // number of tracks of diskette

  uint8_t direction;
  int16_t position;
  bool is_stepping;
  const Note * current_note;
  Time current_halfperiod;
  Time inactive_time;
  uint32_t note_steps_counter;

  void reverse();
  void toggle_stepping();

public:
  FloppyDriveHeadInstrument(uint8_t step_pin, uint8_t direction_pin, uint8_t n_positions = DEFAULT_N_POSITIONS);

  void setup() override;
  void tick(Time time) override;
  void stop() override;
  void reset() override;
  void note_on(const Note & note, uint8_t velocity) override;
  void note_off(const Note & note, uint8_t velocity) override;
};
