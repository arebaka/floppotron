#include "notes_allocator.h"

const NChannel NotesAllocator::N_CHANNELS = 16;

void NotesAllocator::push_playing_note(NChannel channel_number, Note::NPitch pitch, IInstrument * instrument) {
  for (NPlayingNote i = 0; i < polyphony_limit; i++) {
    if (playing_notes[i] == nullptr) {
      playing_notes[i] = new PlayingNote(channel_number, pitch, instrument);
      n_playing_notes++;
      return;
    }
  }
}

PlayingNote * NotesAllocator::pop_playing_note(NChannel channel_number, Note::NPitch pitch) {
  for (NPlayingNote i = 0; i < polyphony_limit; i++) {
    PlayingNote * playing_note = playing_notes[i];
    if (
      playing_note != nullptr
      && playing_note->channel_number == channel_number
      && playing_note->pitch == pitch
    ) {
      playing_notes[i] = nullptr;
      n_playing_notes--;
      return playing_note;
    }
  }

  return nullptr;
}

NotesAllocator::NotesAllocator(NPlayingNote polyphony_limit)
: polyphony_limit(polyphony_limit), n_playing_notes(0)
{
  channel_instruments_map = new IInstrumentsGroup *[N_CHANNELS] {};
  playing_notes = new PlayingNote *[polyphony_limit] {};
}

NotesAllocator::~NotesAllocator() {
  for (NPlayingNote i = 0; i < polyphony_limit; i++) {
    delete playing_notes[i];
  }
  delete [] playing_notes;
  delete [] channel_instruments_map;
}

NPlayingNote NotesAllocator::count_playing_notes() const {
  return n_playing_notes;
}

INotesAllocator & NotesAllocator::dedicate_group(NChannel channel_number, IInstrumentsGroup * group) {
  channel_instruments_map[channel_number] = group;
  return *this;
}

void NotesAllocator::note_on(NChannel channel_number, Note::NPitch pitch, Velocity velocity) {
  if (velocity == 0) {
    return note_off(channel_number, pitch, velocity);
  }

  IInstrumentsGroup * group = channel_instruments_map[channel_number];
  if (group == nullptr || group->is_empty() || n_playing_notes >= polyphony_limit) {
    return;
  }

  IInstrument * instrument = group->pop();
  if (instrument == nullptr) {
    return;
  }

  push_playing_note(channel_number, pitch, instrument);
  instrument->note_on(pitch, velocity);
}

void NotesAllocator::note_off(NChannel channel_number, Note::NPitch pitch, Velocity velocity) {
  IInstrumentsGroup * group = channel_instruments_map[channel_number];
  PlayingNote * playing_note = pop_playing_note(channel_number, pitch);
  if (group == nullptr || playing_note == nullptr) {
    return;
  }

  IInstrument * instrument = playing_note->instrument;
  delete playing_note;

  instrument->note_off(pitch, velocity);
  if (instrument->get_current_pitch() != Note::NULL_PITCH) {
    push_playing_note(channel_number, pitch, instrument);
    return;
  }

  if (!group->is_full()) {
    group->push(instrument);
  }
}
