#include "instrument/floppy_drive_head.h"
#include "instrument/light.h"
#include "instrument_group/queue.h"
#include "notes_allocator/notes_allocator.h"
#include "message_handler/message_handler.h"

using instrument::FloppyDriveHead;
using instrument::Light;
using InstrumentsQueue = instruments_group::Queue;

const Time TICK_LENGTH = 10;
const NInstrument N_MELODIC_INSTRUMENTS = 8;
const NInstrument N_PERCUSSION_INSTRUMENTS = 2;
const NInstrument N_INSTRUMENTS = N_MELODIC_INSTRUMENTS + N_PERCUSSION_INSTRUMENTS;

IInstrument * instruments[N_INSTRUMENTS] {};
NotesAllocator notes_allocator(N_MELODIC_INSTRUMENTS);
MessageHandler message_handler(N_INSTRUMENTS, instruments, &notes_allocator);

void setup() {
  // TODO read config from EEPROM
  instruments[0] = new Light(2);
  instruments[1] = new Light(3);
  instruments[2] = new Light(4);
  instruments[3] = new Light(5);
  instruments[4] = new Light(6);
  instruments[5] = new Light(7);
  instruments[6] = new Light(8);
  instruments[7] = new Light(9);
  instruments[8] = new Light(10);
  instruments[9] = new Light(11);

  IInstrumentsGroup * percussion_group = new InstrumentsQueue(N_PERCUSSION_INSTRUMENTS, instruments);
  IInstrumentsGroup * melodic_group = new InstrumentsQueue(N_MELODIC_INSTRUMENTS, instruments + N_PERCUSSION_INSTRUMENTS);

  notes_allocator
    .dedicate_group(0, melodic_group)
    .dedicate_group(1, melodic_group)
    .dedicate_group(2, melodic_group)
    .dedicate_group(3, melodic_group)
    .dedicate_group(4, melodic_group)
    .dedicate_group(5, melodic_group)
    .dedicate_group(6, melodic_group)
    .dedicate_group(7, melodic_group)
    .dedicate_group(8, melodic_group)
    .dedicate_group(9, percussion_group)
    .dedicate_group(10, percussion_group)
    .dedicate_group(11, melodic_group)
    .dedicate_group(12, melodic_group)
    .dedicate_group(13, melodic_group)
    .dedicate_group(14, melodic_group)
    .dedicate_group(15, melodic_group);

  for (auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->setup();
    }
  }

  Serial.begin(57600);
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t data = (uint8_t) Serial.read();
    Serial.write(data);
    message_handler.handle_byte(data);
  }

  delay(TICK_LENGTH / 1000);
  delayMicroseconds((uint16_t) TICK_LENGTH);
  for (NInstrument i = 0; i < N_INSTRUMENTS; i++) {
    if (instruments[i] != nullptr) {
      instruments[i]->tick(TICK_LENGTH);
    }
  }
}
