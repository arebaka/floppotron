#pragma once
#include "../types.h"

class IMessageHandler {
public:
  virtual void handle_byte(uint8_t payload) = 0;
};
