#pragma once
#include "types.h"

class Note {
public:
  static const Note notes[];

  static char * get_name(uint8_t number);
  static constexpr float get_freq(uint8_t number);
  static constexpr float get_period(uint8_t number);

public:
  const Time period;

  constexpr Note(const char * name, float freq);
  constexpr Note(uint8_t number);
};
