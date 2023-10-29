#pragma once
#include "note.h"
#include "i_instrument.h"

class PlayingNote {
public:
  const NChannel channel_number;
  const Note::NPitch pitch;
  IInstrument * instrument;

  PlayingNote(NChannel channel_number, Note::NPitch pitch, IInstrument * instrument);
};
