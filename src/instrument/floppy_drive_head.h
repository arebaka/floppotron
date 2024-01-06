#pragma once
#include "i_instrument.h"

namespace instrument {
class FloppyDriveHead : public IInstrument {
public:
  typedef int8_t NPosition;
  typedef int8_t Direction;
  typedef int8_t Phase;
  typedef int16_t NStep;

  static const NPosition DEFAULT_N_POSITIONS;
  static const Time TIME_TO_WAIT_AFTER_SETUP;

protected:
  const NPin step_pin;
  const NPin direction_pin;
  const NPosition n_positions;  // number of tracks of diskette
  Direction direction;
  NPosition position;
  Phase phase;
  Time current_halfperiod;
  Time inactive_time;
  NStep note_steps_counter;
  Note::NPitch current_pitch;

  virtual void write_direction();
  virtual void write_phase();

  void reverse();
  void toggle_phase();

public:
  FloppyDriveHead(NPin step_pin, NPin direction_pin, NPosition n_positions = DEFAULT_N_POSITIONS);

  void setup() override;
  void tick(Time time) override;
  void stop() override;
  void reset() override;
  void note_on(Note::NPitch pitch, Velocity velocity) override;
  void note_off(Note::NPitch pitch, Velocity velocity) override;
  Note::NPitch get_current_pitch() const override;
};
}
