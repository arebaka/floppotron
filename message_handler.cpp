#include "message_handler.h"

void MessageHandler::handle_status_meta(uint8_t payload) {
  if (payload == 0xFF) {
    for (NInstrument i = 0; i < n_instruments; i++) {
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
  state = after_status_states_map[command_nibble & 0b00000111];
}

MessageHandler::MessageHandler(NInstrument n_instruments, IInstrument ** instruments, INotesAllocator * allocator)
  : state(State::STATUS), n_instruments(n_instruments), instruments(instruments), allocator(allocator) {}

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
      allocator->note_off(channel_number, params[0], params[1]);
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
      allocator->note_on(channel_number, params[0], params[1]);
      state = State::STATUS;
      break;
    }
    // TODO handle key pressure, controller change, program change, channel pressure, pitch bend
    default:
      state = State::STATUS;
      break;
  }
}
