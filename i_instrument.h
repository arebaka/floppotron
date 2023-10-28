#pragma once
#include "note.h"

class IInstrument {
public:
  virtual void setup() = 0;
  virtual void tick(Time time) = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
  virtual void note_on(const Note & note, uint8_t velocity) = 0;
  virtual void note_off(const Note & note, uint8_t velocity) = 0;
};
