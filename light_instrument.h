#pragma once
#include "i_instrument.h"

class LightInstrument : public IInstrument {
protected:
  const uint8_t pin;

  bool is_lighting;
  const Note * current_note;

public:
  LightInstrument(uint8_t pin);

  void setup() override;
  void tick(Time time) override;
  void stop() override;
  void reset() override;
  void note_on(const Note & note, uint8_t velocity) override;
  void note_off(const Note & note, uint8_t velocity) override;
};
