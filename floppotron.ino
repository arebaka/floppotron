typedef uint32_t Time;

class Note {
public:
  const char * name;
  const float freq;
  const Time period;

  Note(char * name, float freq)
  : name(name), freq(freq), period(1000000 / freq) {}
};

class IInstrument {
public:
  virtual void setup() = 0;
  virtual void tick(Time time) = 0;
  virtual void stop() = 0;
  virtual void reset() = 0;
  virtual void note_on(const Note * note, int8_t velocity) = 0;
  virtual void note_off(const Note * note, int8_t velocity) = 0;
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

  void note_on(const Note * note, int8_t velocity) override {
    if (velocity == 0) {
      note_off(note, velocity);
    }
    else {
      current_note = note;
      current_halfperiod = note->period / 2;
    }
  }

  void note_off(const Note * note, int8_t velocity) override {
    if (note != current_note) {
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

const Note * notes[] {
  // octave -1
  new Note("", 8.18), new Note("", 8.66), new Note("", 9.18), new Note("", 9.72), new Note("", 10.30), new Note("", 10.91),
  new Note("", 11.56), new Note("", 12.25), new Note("", 12.98), new Note("", 13.75), new Note("", 14.57), new Note("", 15.43),
  // octave 0
  new Note("", 16.35), new Note("", 17.32), new Note("", 18.35), new Note("", 19.45), new Note("", 20.60), new Note("", 21.83),
  new Note("", 23.12), new Note("", 24.50), new Note("", 25.96), new Note("A0", 27.50), new Note("A#0", 29.14), new Note("B0", 30.87),
  // octave 1
  new Note("C1", 32.70), new Note("C#1", 34.65), new Note("D1", 36.71), new Note("D#1", 38.89), new Note("E1", 41.20), new Note("F1", 43.65),
  new Note("F#1", 46.25), new Note("G1", 49.00), new Note("G#1", 51.91), new Note("A1", 55.00), new Note("A#1", 58.27), new Note("B1", 61.74),
  // octave 2
  new Note("C2", 65.41), new Note("C#2", 69.30), new Note("D2", 73.42), new Note("D#2", 77.78), new Note("E2", 82.41), new Note("F2", 87.31),
  new Note("F#2", 92.50), new Note("G2", 98.00), new Note("G#2", 103.83), new Note("A2", 110.00), new Note("A#2", 116.54), new Note("B2", 123.47),
  // octave 3
  new Note("C3", 130.81), new Note("C#3", 138.59), new Note("D3", 146.83), new Note("D#3", 155.56), new Note("E3", 164.81), new Note("F3", 174.61),
  new Note("F#3", 185.00), new Note("G3", 196.00), new Note("G#3", 207.65), new Note("A3", 220.00), new Note("A#3", 233.08), new Note("B3", 246.94),
  // octave 4
  new Note("C4", 261.63), new Note("C#4", 277.18), new Note("D4", 293.66), new Note("D#4", 311.13), new Note("E4", 329.63), new Note("F4", 349.23),
  new Note("F#4", 369.99), new Note("G4", 392.00), new Note("G#4", 415.30), new Note("A4", 440.00), new Note("A#4", 466.16), new Note("B4", 493.88),
  // octave 5
  new Note("C5", 523.25), new Note("C#5", 554.37), new Note("D5", 587.33), new Note("D#5", 622.25), new Note("E5", 659.26), new Note("F5", 698.46),
  new Note("F#5", 739.99), new Note("G5", 783.99), new Note("G#5", 830.61), new Note("A5", 880.00), new Note("A#5", 932.33), new Note("B5", 987.77),
  // octave 6
  new Note("C6", 1046.50), new Note("C#6", 1108.73), new Note("D6", 1174.66), new Note("D#6", 1244.51), new Note("E6", 1318.51), new Note("F6", 1396.91),
  new Note("F#6", 1479.98), new Note("G6", 1567.98), new Note("G#6", 1661.22), new Note("A6", 1760.00), new Note("A#6", 1864.66), new Note("B6", 1975.53),
  // octave 7
  new Note("C7", 2093.00), new Note("C#7", 2217.46), new Note("D7", 2349.32), new Note("D#7", 2489.02), new Note("E7", 2637.02), new Note("F7", 2793.83),
  new Note("F#7", 2959.96), new Note("G7", 3135.96), new Note("G#7", 3322.44), new Note("A7", 3520.00), new Note("A#7", 3729.31), new Note("B7", 3951.07),
  // octave 8
  new Note("C8", 4186.01), new Note("C#8", 4434.92), new Note("D8", 4698.64), new Note("D#8", 4978.03), new Note("E8", 5274.04), new Note("F8", 5587.65),
  new Note("F#8", 5919.91), new Note("G8", 6271.93), new Note("G#8", 6644.88), new Note("A8", 7040.00), new Note("A#8", 7458.62), new Note("B8", 7902.13),
  // octave 9
  new Note("C9", 8372.02), new Note("C#9", 8869.84), new Note("D9", 9397.27), new Note("D#9", 9956.06), new Note("E9", 10548.08), new Note("F9", 11175.30),
  new Note("F#9", 11839.82), new Note("G9", 12543.85)
};

const IInstrument * instruments[] = {
  new FloppyDriveHeadInstrument(2, 3),
  new FloppyDriveHeadInstrument(4, 5),
};

// WARN temp dirty hack, need a full balancer
const IInstrument * channel_instruments_map[] = {
  instruments[0], instruments[1], instruments[1], instruments[1],
  nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr
};

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
        for (auto instrument : instruments) {
          instrument->reset();
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
        channel_instruments_map[channel_number]->note_on(notes[args[0]], args[1]);
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
        channel_instruments_map[channel_number]->note_off(notes[args[0]], args[1]);
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
  for (auto instrument : instruments) {
    instrument->setup();
  }
  Serial.begin(57600);
}

void loop() {
  if (Serial.available() > 0) {
    int8_t data = Serial.read() % 256;
  }

  delay(TICK_LENGTH / 1000);
  delayMicroseconds((uint16_t) TICK_LENGTH);
  for (auto instrument : instruments) {
    instrument->tick(TICK_LENGTH);
  }
}
