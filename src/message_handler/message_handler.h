#pragma once
#include "i_message_handler.h"
#include "../notes_allocator/i_notes_allocator.h"

class MessageHandler : public IMessageHandler {
private:
  enum class State : uint8_t {
    STATUS, NOTE_OFF_PITCH, NOTE_OFF_VELOCITY, NOTE_ON_PITCH, NOTE_ON_VELOCITY,
    KEY_PRESSURE_KEY, KEY_PRESSURE_PRESSURE, CONTROLLER_CHANGE_CONTROLLER, CONTROLLER_CHANGE_VALUE,
    PROGRAM_CHANGE_PRESET, CHANNEL_PRESSURE_PRESSURE, PITCH_BEND_LSB, PITCH_BEND_MSB,
    SYSEX, SONG_POSITION_LSB, SONG_POSITION_MSB, SONG_SELECT_NUMBER, BUS_SELECT_NUMBER
  };

  State state;
  const NInstrument n_instruments;
  IInstrument ** instruments;
  INotesAllocator * allocator;
  NChannel channel_number;
  uint8_t params[2];

  void handle_status_meta(uint8_t payload);
  void handle_status(uint8_t payload);

public:
  MessageHandler(NInstrument n_instruments, IInstrument ** instruments, INotesAllocator * allocator);

  void handle_byte(uint8_t payload) override;
};
