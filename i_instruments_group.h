#pragma once
#include "i_instrument.h"

class IInstrumentsGroup {
public:
  virtual NInstrument size() const = 0;
  virtual bool is_empty() const = 0;
  virtual bool is_full() const = 0;
  virtual IInstrument * pop() = 0;
  virtual void push(IInstrument * instrument) = 0;
};
