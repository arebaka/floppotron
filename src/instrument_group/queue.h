#pragma once
#include "i_instruments_group.h"

namespace instruments_group {
class Queue : public IInstrumentsGroup {
private:
  const NInstrument n_instruments;
  NInstrument front_index;
  NInstrument back_index;
  NInstrument current_size;
  IInstrument ** instruments;

public:
  Queue(NInstrument n_instruments, IInstrument ** instruments);  // takes already setuped instruments

  NInstrument size() const override;
  bool is_empty() const override;
  bool is_full() const override;
  IInstrument * pop() override;
  void push(IInstrument * instrument) override;
};
}
