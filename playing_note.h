#pragma once
#include "note.h"
#include "i_instrument.h"

class PlayingNote {
public:
  const uint8_t channel_number;
  const Note & note;
  IInstrument * instrument;
  PlayingNote(uint8_t channel_number, const Note & note, IInstrument * instrument);
};
