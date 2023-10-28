#include "HardwareSerial.h"
#pragma once
#include "i_notes_allocator.h"

class NotesAllocator : public INotesAllocator {
public:
  static const uint8_t N_CHANNELS;

private:
  const uint16_t polyphony_limit;

  IInstrumentsGroup ** channel_instruments_map;
  PlayingNote ** playing_notes;
  int16_t n_playing_notes;

  void push_playing_note(uint8_t channel_number, const Note & note, IInstrument * instrument);
  PlayingNote * pop_playing_note(uint8_t channel_number, const Note & note);

public:
  NotesAllocator(uint16_t polyphony_limit);
  uint16_t count_playing_notes() const override;
  void dedicate_group(uint8_t channel_number, IInstrumentsGroup * group) override;
  void note_on(uint8_t channel_number, const Note & note, uint8_t velocity) override;
  void note_off(uint8_t channel_number, const Note & note, uint8_t velocity) override;
};
