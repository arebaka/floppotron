#pragma once
#include "i_instruments_group.h"

namespace instruments_group {
class Stack : public IInstrumentsGroup {
private:
  const NInstrument n_instruments;
  NInstrument current_size;
  IInstrument ** instruments;

public:
  Stack(NInstrument n_instruments, IInstrument ** instruments);  // takes already setuped instruments

  NInstrument size() const override;
  bool is_empty() const override;
  bool is_full() const override;
  IInstrument * pop() override;
  void push(IInstrument * instrument) override;
};
}
