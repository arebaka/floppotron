#pragma once
#include "../instrument_group/i_instruments_group.h"

class INotesAllocator {
public:
  virtual NPlayingNote count_playing_notes() const = 0;
  virtual INotesAllocator & dedicate_group(NChannel channel_number, IInstrumentsGroup & group) = 0;
  virtual void note_on(NChannel channel_number, Note::NPitch pitch, Velocity velocity) = 0;
  virtual void note_off(NChannel channel_number, Note::NPitch pitch, Velocity velocity) = 0;
};
