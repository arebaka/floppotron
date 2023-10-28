#include "types.h"

#include "i_instrument.h"
#include "floppy_drive_head_instrument.h"
#include "light_instrument.h"
#include "i_message_handler.h"
#include "message_handler.h"

const Time TICK_LENGTH = 40;

IInstrument * instruments[16] {};
IInstrument * channel_instruments_map[16] {};
MessageHandler message_handler(16, instruments, channel_instruments_map);

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
  for (auto & instrument : instruments) {
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
