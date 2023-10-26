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
    int8_t octave = number / 12 - 1;
    int8_t offset = number % 12;

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

public:
  const Time period;

  Note(const char * name, float freq)
  : period(1000000 / freq) {}

  Note(uint8_t number)
  : period(1000000 / 440 / pow(2.0, (number - 69) / 12)) {}
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
  virtual void note_on(const Note & note, int8_t velocity) = 0;
  virtual void note_off(const Note & note, int8_t velocity) = 0;
};

class FloppyDriveHeadInstrument : public IInstrument {
public:
  static const uint16_t DEFAULT_N_POSITIONS = 80;  // for general 3.5 and 5.25 720K+
  static const uint16_t TIME_TO_WAIT_AFTER_RESET = 100;

protected:
  const uint8_t direction_pin;
  const uint8_t step_pin;
  const uint16_t n_positions;  // number of tracks of diskette

  uint8_t direction;
  int16_t position;
  bool is_stepping;
  const Note * current_note;
  Time current_halfperiod;
  Time inactive_time;

  void toggle_stepping() {
    // reverse if end has been reached
    if (
      (direction == HIGH && position >= n_positions - 1)
      || (direction == LOW && position <= 0)
    ) {
      direction = direction == HIGH ? LOW : HIGH;
      digitalWrite(direction_pin, direction);
    }

    if (is_stepping) {
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
  FloppyDriveHeadInstrument(uint8_t direction_pin, uint8_t step_pin, uint8_t n_positions = DEFAULT_N_POSITIONS)
  : direction_pin(direction_pin), step_pin(step_pin), n_positions(n_positions) {}

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
  }

  void reset() {
    stop();
    digitalWrite(direction_pin, LOW);
    direction = LOW;

    for (uint16_t i = 0; i < n_positions; i++) {
      digitalWrite(step_pin, HIGH);
      delay(5);
      digitalWrite(step_pin, LOW);
      delay(5);
    }

    position = 0;
  }

  void note_on(const Note & note, int8_t velocity) override {
    if (velocity == 0) {
      note_off(note, velocity);
    }
    else {
      current_note = &note;
      current_halfperiod = note.period / 2;
    }
  }

  void note_off(const Note & note, int8_t velocity) override {
    if (&note != current_note) {
      return;
    }
    current_note = nullptr;
    current_halfperiod = 0;
    inactive_time = 0;

    // reverse to direction with more available steps to hold a next note longer
    if (
      (direction == HIGH && position >= n_positions / 2)
      || (direction == LOW && position < n_positions / 2)
    ) {
      direction = direction == HIGH ? LOW : HIGH;
      digitalWrite(direction_pin, direction);
    }
  }
};

const IInstrument * instruments[16] {};
const IInstrument * channel_instruments_map[16] {};

class IMessageHandler {
public:
  virtual void handle_byte(int8_t payload) = 0;
};

class MessageHandler {
public:
  enum class State { STATUS, NOTE_ON_PITCH, NOTE_ON_VELOCITY, NOTE_OFF_PITCH, NOTE_OFF_VELOCITY };

private:
  State state;
  int8_t channel_number;
  int8_t args[4];

  void handle_status_byte(int8_t payload) {
    int8_t command_nibble = payload >> 4;
    if (!command_nibble) {
      return;
    }
    channel_number = payload & 0xF;

    switch (command_nibble) {
    case 0x8: {
      state = State::NOTE_ON_PITCH;
      break;
    }
    case 0x9: {
      state = State::NOTE_OFF_PITCH;
      break;
    }
    // TODO handle key pressure, controller change, program change, channel pressure, pitch bend
    case 0xF: {
      if (payload == 0xFF) {
        for (const auto & instrument : instruments) {
          if (instrument != nullptr) {
            instrument->reset();
          }
        }
      }
      // TODO handle other system messages
      break;
    }
    default:
      break;
    }
  }

public:
  MessageHandler() : state(State::STATUS) {}
  void handle_byte(int8_t payload) {
    switch (state) {
      case State::STATUS: {
        handle_status_byte(payload);
        break;
      }
      case State::NOTE_ON_PITCH: {
        args[0] = payload;
        state = State::NOTE_ON_VELOCITY;
        break;
      }
      case State::NOTE_ON_VELOCITY: {
        args[1] = payload;
        channel_instruments_map[channel_number]->note_on(Note::notes[args[0]], args[1]);
        state = State::STATUS;
        break;
      }
      case State::NOTE_OFF_PITCH: {
        args[0] = payload;
        state = State::NOTE_OFF_VELOCITY;
        break;
      }
      case State::NOTE_OFF_VELOCITY: {
        args[1] = payload;
        channel_instruments_map[channel_number]->note_off(Note::notes[args[0]], args[1]);
        state = State::STATUS;
        break;
      }
      default:
        break;
    }
  }
};

const Time TICK_LENGTH = 40;

void setup() {
  // TODO read config from EEPROM
  instruments[0] = new FloppyDriveHeadInstrument(2, 3);
  instruments[1] = new FloppyDriveHeadInstrument(4, 5);
  for (const auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->setup();
    }
  }

  // WARN: temp dirty hack, need a full allocator
  channel_instruments_map[0] = instruments[0];
  channel_instruments_map[1] = instruments[1];
  channel_instruments_map[2] = instruments[1];
  channel_instruments_map[3] = instruments[1];

  Serial.begin(57600);
}

void loop() {
  if (Serial.available() > 0) {
    int16_t data = Serial.read() % 256;
  }

  delay(TICK_LENGTH / 1000);
  delayMicroseconds((uint16_t) TICK_LENGTH);
  for (const auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->tick(TICK_LENGTH);
    }
  }
}
