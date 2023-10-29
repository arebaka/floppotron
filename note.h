#pragma once
#include "types.h"

class Note {
public:
  typedef int8_t NPitch;
  typedef uint8_t NOctave;

  static const NPitch N_PITCHES;
  static const NPitch NULL_PITCH;
  static const Note pitches[];

  static constexpr NOctave get_octave(NPitch pitch);
  static constexpr float get_freq(NPitch pitch);
  static constexpr float get_period(NPitch pitch);
  static const char * get_name(NPitch pitch);

public:
  const Time period;

  constexpr Note(NPitch pitch);
};
