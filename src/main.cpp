#include "util/shift_register.h"
#include "instrument/floppy_drive_head.h"
#include "instrument/shifted_floppy_drive_head.h"
#include "instrument/light.h"
#include "instrument/choir.h"
#include "instrument_group/queue.h"
#include "instrument_group/stack.h"
#include "notes_allocator/notes_allocator.h"
#include "message_handler/message_handler.h"

using util::ShiftRegister;
using instrument::FloppyDriveHead;
using instrument::ShiftedFloppyDriveHead;
using instrument::Light;
using instrument::Choir;
using InstrumentsQueue = instruments_group::Queue;
using InstrumentsStack = instruments_group::Stack;

const NInstrument N_MELODIC_INSTRUMENTS = 11;
const NInstrument N_PERCUSSION_INSTRUMENTS = 0;
const NInstrument N_INSTRUMENTS = N_MELODIC_INSTRUMENTS + N_PERCUSSION_INSTRUMENTS;

IInstrument * instruments[N_INSTRUMENTS] {};
NotesAllocator notes_allocator(N_MELODIC_INSTRUMENTS);
MessageHandler message_handler(N_INSTRUMENTS, instruments, notes_allocator);
Time pulse;

void setup() {
  // TODO read config from EEPROM
  ShiftRegister shift_register(11, 13, 12);

  // instruments[5] = new FloppyDriveHead(2, 3);
  // instruments[0] = new FloppyDriveHead(4, 5);
  // instruments[3] = new FloppyDriveHead(6, 7);
  // instruments[4] = new FloppyDriveHead(8, 9);
  // instruments[1] = new FloppyDriveHead(10, A0);
  // instruments[7] = new FloppyDriveHead(A1, A2);
  // instruments[6] = new FloppyDriveHead(A3, A4);
  // instruments[2] = new ShiftedFloppyDriveHead(shift_register, 0, 1);
  // instruments[8] = new ShiftedFloppyDriveHead(shift_register, 2, 3);
  // instruments[9] = new ShiftedFloppyDriveHead(shift_register, 4, 5);
  // instruments[10] = new ShiftedFloppyDriveHead(shift_register, 6, 7);

  // instruments[0] = new FloppyDriveHead(2, 3);
  // instruments[4] = new FloppyDriveHead(4, 5);
  // instruments[5] = new FloppyDriveHead(6, 7);
  // instruments[6] = new FloppyDriveHead(8, 9);
  // instruments[1] = new FloppyDriveHead(10, A0);
  // instruments[2] = new FloppyDriveHead(A1, A2);
  // instruments[3] = new FloppyDriveHead(A3, A4);
  // instruments[7] = new ShiftedFloppyDriveHead(shift_register, 0, 1);
  // instruments[8] = new ShiftedFloppyDriveHead(shift_register, 2, 3);
  // instruments[9] = new ShiftedFloppyDriveHead(shift_register, 4, 5);
  // instruments[10] = new ShiftedFloppyDriveHead(shift_register, 6, 7);

  instruments[3] = new FloppyDriveHead(2, 3);
  instruments[9] = new FloppyDriveHead(4, 5);
  instruments[8] = new FloppyDriveHead(6, 7);
  instruments[7] = new FloppyDriveHead(8, 9);
  instruments[6] = new FloppyDriveHead(10, A0);
  instruments[5] = new FloppyDriveHead(A1, A2);
  instruments[4] = new FloppyDriveHead(A3, A4);
  instruments[10] = new ShiftedFloppyDriveHead(shift_register, 0, 1);
  instruments[2] = new ShiftedFloppyDriveHead(shift_register, 2, 3);
  instruments[1] = new ShiftedFloppyDriveHead(shift_register, 4, 5);
  instruments[0] = new ShiftedFloppyDriveHead(shift_register, 6, 7);

  IInstrument * choirs[3] {};
  choirs[0] = new Choir(4, instruments);
  choirs[1] = new Choir(3, instruments + 4);
  choirs[2] = new Choir(4, instruments + 7);

  InstrumentsStack percussion_group(N_PERCUSSION_INSTRUMENTS, instruments);
  InstrumentsStack melodic_group(N_MELODIC_INSTRUMENTS, instruments + N_PERCUSSION_INSTRUMENTS);
  // InstrumentsStack melodic_group(3, choirs);

  // InstrumentsStack melodic_group_1(1, instruments + N_PERCUSSION_INSTRUMENTS + 0);
  // InstrumentsStack melodic_group_2(1, instruments + N_PERCUSSION_INSTRUMENTS + 1);
  // InstrumentsStack melodic_group_3(1, instruments + N_PERCUSSION_INSTRUMENTS + 2);
  // InstrumentsStack melodic_group_4(1, instruments + N_PERCUSSION_INSTRUMENTS + 3);
  // InstrumentsStack melodic_group_5(1, instruments + N_PERCUSSION_INSTRUMENTS + 4);
  // InstrumentsStack melodic_group_6(1, instruments + N_PERCUSSION_INSTRUMENTS + 5);
  // InstrumentsStack melodic_group_7(1, instruments + N_PERCUSSION_INSTRUMENTS + 6);
  // InstrumentsStack melodic_group_8(1, instruments + N_PERCUSSION_INSTRUMENTS + 7);

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
    .dedicate_group(15, melodic_group)
    ;

  for (auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->setup();
    }
  }

  pulse = micros();
  Serial.begin(57600);
}

void loop() {
  while (Serial.available() > 0) {
    uint8_t data = (uint8_t) Serial.read();
    Serial.write(data);
    message_handler.handle_byte(data);
  }

  Time time = micros();
  Time tick_length = time - pulse;
  pulse = time;

  for (auto & instrument : instruments) {
    if (instrument != nullptr) {
      instrument->tick(tick_length);
    }
  }
}
