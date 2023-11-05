#pragma once
#include "i_instrument.h"

namespace instrument {
class Light : public IInstrument {
protected:
  const NPin pin;
  bool is_lighting;
  Note::NPitch current_pitch;

public:
  Light(NPin pin);

  void setup() override;
  void tick(Time time) override;
  void stop() override;
  void reset() override;
  void note_on(Note::NPitch pitch, Velocity velocity) override;
  void note_off(Note::NPitch pitch, Velocity velocity) override;
  Note::NPitch get_current_pitch() const override;
};
}
