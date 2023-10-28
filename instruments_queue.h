#pragma once
#include "i_instruments_group.h"
#include "i_instrument.h"

class InstrumentsQueue : public IInstrumentsGroup {
private:
  const uint16_t n_instruments;
  IInstrument ** instruments;
  uint16_t front_index;
  uint16_t back_index;
  uint16_t current_size;

public:
  InstrumentsQueue(uint16_t n_instruments, IInstrument ** instruments);  // takes already setuped instruments
  uint16_t size() const override;
  bool is_empty() const override;
  bool is_full() const override;
  IInstrument * pop() override;
  void push(IInstrument * instrument) override;
};
