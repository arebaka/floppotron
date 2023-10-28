#include "playing_note.h"

PlayingNote::PlayingNote(uint8_t channel_number, const Note & note, IInstrument * instrument)
  : channel_number(channel_number), note(note), instrument(instrument) {}
