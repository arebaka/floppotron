#pragma once
#include "note.h"

class IInstrument {
public:
  virtual void setup() = 0;
  virtual void tick(Time time) = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
  virtual void note_on(Note::NPitch pitch, Velocity velocity) = 0;
  virtual void note_off(Note::NPitch pitch, Velocity velocity) = 0;
  virtual Note::NPitch get_current_pitch() const = 0;
};
