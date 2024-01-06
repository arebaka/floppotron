#pragma once
#include "i_notes_allocator.h"
#include "../note/playing_note.h"

class NotesAllocator : public INotesAllocator {
public:
  static const NChannel N_CHANNELS;

private:
  const NPlayingNote polyphony_limit;
  NPlayingNote n_playing_notes;
  IInstrumentsGroup ** channel_instruments_map;
  PlayingNote ** playing_notes;

  void push_playing_note(NChannel channel_number, Note::NPitch pitch, IInstrument * instrument);
  const PlayingNote * pop_playing_note(NChannel channel_number, Note::NPitch pitch);

public:
  NotesAllocator(NPlayingNote polyphony_limit);
  ~NotesAllocator();

  NPlayingNote count_playing_notes() const override;
  INotesAllocator & dedicate_group(NChannel channel_number, IInstrumentsGroup & group) override;
  void note_on(NChannel channel_number, Note::NPitch pitch, Velocity velocity) override;
  void note_off(NChannel channel_number, Note::NPitch pitch, Velocity velocity) override;
};
