#pragma once
#include "i_instruments_group.h"
#include "playing_note.h"

class INotesAllocator {
public:
  virtual uint16_t count_playing_notes() const = 0;
  virtual void dedicate_group(uint8_t channel_number, IInstrumentsGroup * group) = 0;
  virtual void note_on(uint8_t channel_number, const Note & note, uint8_t velocity) = 0;
  virtual void note_off(uint8_t channel_number, const Note & note, uint8_t velocity) = 0;
};
