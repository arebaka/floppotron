#include "playing_note.h"

PlayingNote::PlayingNote(NChannel channel_number, Note::NPitch pitch, IInstrument * instrument)
: channel_number(channel_number), pitch(pitch), instrument(instrument) {}
