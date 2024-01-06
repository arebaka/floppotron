#pragma once
#include "i_instrument.h"

namespace instrument {
class Choir : public IInstrument {
protected:
  const NInstrument n_instruments;
  IInstrument ** instruments;
  Note::NPitch current_pitch;

public:
  Choir(NInstrument n_instruments, IInstrument ** instruments);

  void setup() override;
  void tick(Time time) override;
  void stop() override;
  void reset() override;
  void note_on(Note::NPitch pitch, Velocity velocity) override;
  void note_off(Note::NPitch pitch, Velocity velocity) override;
  Note::NPitch get_current_pitch() const override;
};
}
