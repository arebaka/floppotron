#include "i_instrument.h"
#include "i_instruments_group.h"
#include "i_notes_allocator.h"
#include "i_message_handler.h"
#include "floppy_drive_head_instrument.h"
#include "light_instrument.h"
#include "instruments_queue.h"
#include "notes_allocator.h"
#include "message_handler.h"

const Time TICK_LENGTH = 10;
const NInstrument N_INSTRUMENTS = 10;

IInstrument ** instruments;
INotesAllocator * notes_allocator;
IMessageHandler * message_handler;

void setup() {
  instruments = new IInstrument *[N_INSTRUMENTS];
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

  for (NInstrument i = 0; i < N_INSTRUMENTS; i++) {
    if (instruments[i] != nullptr) {
      instruments[i]->setup();
    }
  }

  IInstrumentsGroup * group = new InstrumentsQueue(N_INSTRUMENTS, instruments);
  notes_allocator = new NotesAllocator(N_INSTRUMENTS);

  // TODO read config from EEPROM
  notes_allocator->dedicate_group(0, group);
  notes_allocator->dedicate_group(1, group);
  notes_allocator->dedicate_group(2, group);
  notes_allocator->dedicate_group(3, group);
  notes_allocator->dedicate_group(4, group);
  notes_allocator->dedicate_group(5, group);
  notes_allocator->dedicate_group(6, group);
  notes_allocator->dedicate_group(7, group);
  notes_allocator->dedicate_group(8, group);
  notes_allocator->dedicate_group(9, group);
  notes_allocator->dedicate_group(10, group);
  notes_allocator->dedicate_group(11, group);
  notes_allocator->dedicate_group(12, group);
  notes_allocator->dedicate_group(13, group);
  notes_allocator->dedicate_group(14, group);
  notes_allocator->dedicate_group(15, group);

  message_handler = new MessageHandler(N_INSTRUMENTS, instruments, notes_allocator);

  Serial.begin(57600);
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t data = (uint8_t) Serial.read();
    Serial.write(data);
    message_handler->handle_byte(data);
  }

  delay(TICK_LENGTH / 1000);
  delayMicroseconds((uint16_t) TICK_LENGTH);
  for (NInstrument i = 0; i < N_INSTRUMENTS; i++) {
    if (instruments[i] != nullptr) {
      instruments[i]->tick(TICK_LENGTH);
    }
  }
}
