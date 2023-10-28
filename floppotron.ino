typedef uint32_t Time;

class Note {
public:
  static const Note notes[];

  static char * get_name(uint8_t number) {
    static const char prefixes[] = "CDEFGAB";
    if (number < 21 || number > 127) {
      return nullptr;
    }

    char res[4] {};
    uint8_t octave = number / 12 - 1;
    uint8_t offset = number % 12;

    if (offset == 1 || offset == 3 || offset == 6 || offset == 8 || offset == 10) {
      --offset;  // for C#, D#, F#, G#, A#
      res[1] = '#';
      res[2] = '0' + octave;
    }
    else {
      res[1] = '0' + octave;
    }
    res[0] = prefixes[offset];

    return res;
  }

  static constexpr float get_freq(uint8_t number) {
    return 440.0 * pow(2, (float) (number - 69) / 12);
  }

  static constexpr float get_period(uint8_t number) {
    return 1000000.0 / get_freq(number);
  }

public:
  const Time period;

  Note(const char * name, float freq)
  : period(1000000 / freq) {}

  constexpr Note(uint8_t number)
  : period(get_period(number)) {}
};

const Note Note::notes[] = {
    // octave -1
    Note(0), Note(1), Note(2), Note(3), Note(4), Note(5),
    Note(6), Note(7), Note(8), Note(9), Note(10), Note(11),
    // octave 0
    Note(12), Note(13), Note(14), Note(15), Note(16), Note(17),
    Note(18), Note(19), Note(20), Note(21), Note(22), Note(23),
    // octave 1
    Note(24), Note(25), Note(26), Note(27), Note(28), Note(29),
    Note(30), Note(31), Note(32), Note(33), Note(34), Note(35),
    // octave 2
    Note(36), Note(37), Note(38), Note(39), Note(40), Note(41),
    Note(42), Note(43), Note(44), Note(45), Note(46), Note(47),
    // octave 3
    Note(48), Note(49), Note(50), Note(51), Note(52), Note(53),
    Note(54), Note(55), Note(56), Note(57), Note(58), Note(59),
    // octave 4
    Note(60), Note(61), Note(62), Note(63), Note(64), Note(65),
    Note(66), Note(67), Note(68), Note(69), Note(70), Note(71),
    // octave 5
    Note(72), Note(73), Note(74), Note(75), Note(76), Note(77),
    Note(78), Note(79), Note(80), Note(81), Note(82), Note(83),
    // octave 6
    Note(84), Note(85), Note(86), Note(87), Note(28), Note(89),
    Note(90), Note(91), Note(92), Note(93), Note(94), Note(95),
    // octave 7
    Note(96), Note(97), Note(98), Note(99), Note(100), Note(101),
    Note(102), Note(103), Note(104), Note(105), Note(106), Note(107),
    // octave 8
    Note(108), Note(109), Note(110), Note(111), Note(112), Note(113),
    Note(114), Note(115), Note(116), Note(117), Note(118), Note(119),
    // octave 9
    Note(120), Note(121), Note(122), Note(123), Note(124), Note(125),
    Note(126), Note(127)
  };

class IInstrument {
public:
  virtual void setup() = 0;
  virtual void tick(Time time) = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
  virtual void note_on(const Note & note, uint8_t velocity) = 0;
  virtual void note_off(const Note & note, uint8_t velocity) = 0;
};

class FloppyDriveHeadInstrument : public IInstrument {
public:
  static const uint16_t DEFAULT_N_POSITIONS = 80;  // for general 3.5 and 5.25 720K+
  static const uint16_t TIME_TO_WAIT_AFTER_RESET = 100;

protected:
  const uint8_t step_pin;
  const uint8_t direction_pin;
  const uint16_t n_positions;  // number of tracks of diskette

  uint8_t direction;
  int16_t position;
  bool is_stepping;
  const Note * current_note;
  Time current_halfperiod;
  Time inactive_time;
  uint32_t note_steps_counter;

  void reverse() {
    direction = direction == HIGH ? LOW : HIGH;
    digitalWrite(direction_pin, direction);
  }

  void toggle_stepping() {
    // reverse if end has been reached
    int16_t steps_left = direction == HIGH ? n_positions - position - 1 : position;
    if (steps_left <= 0) {
      reverse();
    }

    if (is_stepping) {
      ++note_steps_counter;
      if (direction == HIGH) {
        ++position;
      } else {
        --position;
      }
    }

    digitalWrite(step_pin, is_stepping ? LOW : HIGH);
    is_stepping = !is_stepping;
  }

public:
  FloppyDriveHeadInstrument(uint8_t step_pin, uint8_t direction_pin, uint8_t n_positions = DEFAULT_N_POSITIONS)
  : step_pin(step_pin), direction_pin(direction_pin), n_positions(n_positions) {}

  void setup() override {
    pinMode(direction_pin, OUTPUT);
    pinMode(step_pin, OUTPUT);
    reset();
    delay(TIME_TO_WAIT_AFTER_RESET);  // wait for safety
  }

  void tick(Time time) override {
    if (current_note == nullptr) {
      return;
    }

    inactive_time += time;
    if (inactive_time >= current_halfperiod) {
      toggle_stepping();
      inactive_time %= current_halfperiod;
    }
  }

  void stop() override {
    digitalWrite(step_pin, LOW);
    is_stepping = false;
    current_note = nullptr;
    current_halfperiod = 0;
    inactive_time = 0;
    note_steps_counter = 0;
  }

  void reset() {
    stop();
    digitalWrite(direction_pin, LOW);
    direction = LOW;

    for (uint16_t i = 0; i < n_positions; i++) {
      digitalWrite(step_pin, HIGH);
      delay(1);
      digitalWrite(step_pin, LOW);
      delay(1);
    }

    position = 0;
  }

  void note_on(const Note & note, uint8_t velocity) override {
    if (velocity == 0) {
      return note_off(note, velocity);
    }
    current_note = &note;
    current_halfperiod = note.period / 2;
  }

  void note_off(const Note & note, uint8_t velocity) override {
    if (&note != current_note) {
      return;
    }
    uint32_t steps_counter = note_steps_counter;  // save the value
    stop();

    // reverse to direction with more available steps to hold a next note without reversion if it is the same in duration
    int16_t steps_left = direction == HIGH ? n_positions - position - 1 : position;
    if (steps_left < steps_counter && n_positions - steps_left > steps_counter) {
      reverse();
    }
  }
};

class LightInstrument : public IInstrument {
protected:
  const uint8_t pin;

  bool is_lighting;
  const Note * current_note;

public:
  LightInstrument(uint8_t pin)
  : pin(pin) {}

  void setup() override {
    pinMode(pin, OUTPUT);
    reset();
  }

  virtual void tick(Time time) override {}

  virtual void stop() override {
    digitalWrite(pin, LOW);
    is_lighting = false;
    current_note = nullptr;
  }
  virtual void reset() override {
    stop();
  }
  virtual void note_on(const Note & note, uint8_t velocity) override {
    if (velocity == 0) {
      return note_off(note, velocity);
    }

    current_note = &note;
    digitalWrite(pin, HIGH);
    is_lighting = true;
  }
  virtual void note_off(const Note & note, uint8_t velocity) override {
    if (&note != current_note) {
      return;
    }
    stop();
  }
};

const IInstrument * instruments[16] {};
const IInstrument * channel_instruments_map[16] {};

class IMessageHandler {
public:
  virtual void handle_byte(uint8_t payload) = 0;
};

class MessageHandler {
private:
  enum class State {
    STATUS, NOTE_OFF_PITCH, NOTE_OFF_VELOCITY, NOTE_ON_PITCH, NOTE_ON_VELOCITY,
    KEY_PRESSURE_KEY, KEY_PRESSURE_PRESSURE, CONTROLLER_CHANGE_CONTROLLER, CONTROLLER_CHANGE_VALUE,
    PROGRAM_CHANGE_PRESET, CHANNEL_PRESSURE_PRESSURE, PITCH_BEND_LSB, PITCH_BEND_MSB,
    SYSEX, SONG_POSITION_LSB, SONG_POSITION_MSB, SONG_SELECT_NUMBER, BUS_SELECT_NUMBER
  };

  State state;
  IInstrument * instrument;
  uint8_t channel_number;
  uint8_t params[2];

  void handle_status_meta(uint8_t payload) {
    if (payload == 0xFF) {
      for (const auto & instrument : instruments) {
        if (instrument != nullptr) {
          instrument->reset();
        }
      }
    }
    // TODO handle other system messages
  }

  void handle_status(uint8_t payload) {
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

public:
  MessageHandler() : state(State::STATUS) {}

  void handle_byte(uint8_t payload) {
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
};

const Time TICK_LENGTH = 40;
const MessageHandler message_handler;

void setup() {
  // TODO read config from EEPROM
  instruments[0] = new LightInstrument(2);
  instruments[1] = new LightInstrument(3);
  instruments[2] = new LightInstrument(4);
  instruments[3] = new LightInstrument(5);
  instruments[4] = new LightInstrument(6);
  instruments[5] = new LightInstrument(7);
  instruments[6] = new LightInstrument(8);
  instruments[7] = new LightInstrument(9);
  instruments[8] = new LightInstrument(10);
  instruments[9] = new LightInstrument(11);
  for (const auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->setup();
    }
  }

  // WARN: temp dirty hack, need a full allocator
  channel_instruments_map[0] = instruments[0];
  channel_instruments_map[1] = instruments[1];
  channel_instruments_map[2] = instruments[2];
  channel_instruments_map[3] = instruments[3];
  channel_instruments_map[4] = instruments[4];
  channel_instruments_map[5] = instruments[5];
  channel_instruments_map[6] = instruments[6];
  channel_instruments_map[9] = instruments[7];
  channel_instruments_map[10] = instruments[8];
  channel_instruments_map[11] = instruments[9];

  Serial.begin(57600);
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t data = (uint8_t) Serial.read();
    message_handler.handle_byte(data);
    Serial.write(data);
  }

  delay(TICK_LENGTH / 1000);
  delayMicroseconds((uint16_t) TICK_LENGTH);
  for (const auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->tick(TICK_LENGTH);
    }
  }
}
