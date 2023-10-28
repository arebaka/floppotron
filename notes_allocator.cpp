#include "notes_allocator.h"

const uint8_t NotesAllocator::N_CHANNELS = 16;

void NotesAllocator::push_playing_note(uint8_t channel_number, const Note & note, IInstrument * instrument) {
  for (uint16_t i = 0; i < polyphony_limit; i++) {
    if (playing_notes[i] == nullptr) {
      playing_notes[i] = new PlayingNote(channel_number, note, instrument);
      n_playing_notes++;
      return;
    }
  }
}

PlayingNote * NotesAllocator::pop_playing_note(uint8_t channel_number, const Note & note) {
  for (uint16_t i = 0; i < polyphony_limit; i++) {
    if (
      playing_notes[i] != nullptr
      && playing_notes[i]->channel_number == channel_number
      && &playing_notes[i]->note == &note
    ) {
      PlayingNote * playing_note = playing_notes[i];
      playing_notes[i] = nullptr;
      n_playing_notes--;
      return playing_note;
    }
  }

  return nullptr;
}

NotesAllocator::NotesAllocator(uint16_t polyphony_limit)
  : polyphony_limit(polyphony_limit), n_playing_notes(0)
{
  channel_instruments_map = new IInstrumentsGroup *[N_CHANNELS] {};
  playing_notes = new PlayingNote *[polyphony_limit] {};
}

uint16_t NotesAllocator::count_playing_notes() const {
  return n_playing_notes;
}

void NotesAllocator::dedicate_group(uint8_t channel_number, IInstrumentsGroup * group) {
  channel_instruments_map[channel_number] = group;
}

void NotesAllocator::note_on(uint8_t channel_number, const Note & note, uint8_t velocity) {
  IInstrumentsGroup * group = channel_instruments_map[channel_number];
  if (group == nullptr || group->is_empty() || n_playing_notes >= polyphony_limit) {
    return;
  }

  IInstrument * instrument = group->pop();
  while (instrument->get_current_note() != nullptr) {
    group->push(instrument);
    instrument = group->pop();
  }

  push_playing_note(channel_number, note, instrument);
  instrument->note_on(note, velocity);
}

void NotesAllocator::note_off(uint8_t channel_number, const Note & note, uint8_t velocity) {
  IInstrumentsGroup * group = channel_instruments_map[channel_number];
  PlayingNote * playing_note = pop_playing_note(channel_number, note);
  if (group == nullptr || playing_note == nullptr) {
    return;
  }

  IInstrument * instrument = playing_note->instrument;
  delete playing_note;

  instrument->note_off(note, velocity);
  if (instrument->get_current_note() != nullptr) {
    push_playing_note(channel_number, note, instrument);
    return;
  }

  if (!group->is_full()) {
    group->push(instrument);
  }
}
