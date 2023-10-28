#include <stdint.h>
#include "message_handler.h"

void MessageHandler::handle_status_meta(uint8_t payload) {
  if (payload == 0xFF) {
    for (uint16_t i = 0; i < n_instruments; i++) {
      if (instruments[i] != nullptr) {
        instruments[i]->reset();
      }
    }
  }
  // TODO handle other system messages
}

void MessageHandler::handle_status(uint8_t payload) {
  static const State after_status_states_map[7] {
    State::NOTE_OFF_PITCH, State::NOTE_ON_PITCH, State::KEY_PRESSURE_KEY,
    State::CONTROLLER_CHANGE_CONTROLLER, State::PROGRAM_CHANGE_PRESET,
    State::CHANNEL_PRESSURE_PRESSURE, State::PITCH_BEND_LSB
  };

  if (payload & 0b10000000 == 0) {
    return;  // not a status
  }

  uint8_t command_nibble = payload >> 4;
  if (command_nibble == 0xF) {
    return handle_status_meta(payload);
  }

  channel_number = payload & 0b00001111;
  instrument = channel_instruments_map[channel_number];
  state = after_status_states_map[command_nibble & 0b00000111];
}

MessageHandler::MessageHandler(uint16_t n_instruments, IInstrument ** instruments, IInstrument ** channel_instruments_map)
  : n_instruments(n_instruments), instruments(instruments), channel_instruments_map(channel_instruments_map), state(State::STATUS) {}

void MessageHandler::handle_byte(uint8_t payload) {
  if (payload & 0b10000000) {
    state = State::STATUS;  // reset message if it is a status byte
  }

  switch (state) {
    case State::STATUS: {
      return handle_status(payload);
    }
    case State::NOTE_OFF_PITCH: {
      params[0] = payload;
      state = State::NOTE_OFF_VELOCITY;
      break;
    }
    case State::NOTE_OFF_VELOCITY: {
      params[1] = payload;
      if (instrument != nullptr) {
        instrument->note_off(Note::notes[params[0]], params[1]);
      }
      state = State::STATUS;
      break;
    }
    case State::NOTE_ON_PITCH: {
      params[0] = payload;
      state = State::NOTE_ON_VELOCITY;
      break;
    }
    case State::NOTE_ON_VELOCITY: {
      params[1] = payload;
      if (instrument != nullptr) {
        instrument->note_on(Note::notes[params[0]], params[1]);
      }
      state = State::STATUS;
      break;
    }
    // TODO handle key pressure, controller change, program change, channel pressure, pitch bend
    default:
      state = State::STATUS;
      break;
  }
}
