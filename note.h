#pragma once
#include "types.h"

class Note {
public:
  static const uint8_t N_PITCHES;
  static const Note pitches[];

  static const char * get_name(uint8_t pitch);
  static constexpr uint8_t get_octave(uint8_t pitch);
  static constexpr float get_freq(uint8_t pitch);
  static constexpr float get_period(uint8_t pitch);

public:
  const uint8_t pitch;
  const Time period;

  constexpr Note(uint8_t pitch);
};
