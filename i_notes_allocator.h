#pragma once
#include "i_instruments_group.h"
#include "playing_note.h"

class INotesAllocator {
public:
  virtual NPlayingNote count_playing_notes() const = 0;
  virtual void dedicate_group(NChannel channel_number, IInstrumentsGroup * group) = 0;
  virtual void note_on(NChannel channel_number, Note::NPitch pitch, Velocity velocity) = 0;
  virtual void note_off(NChannel channel_number, Note::NPitch pitch, Velocity velocity) = 0;
};
