#include "note.h"

const Note::NPitch Note::N_PITCHES = 128;
const Note::NPitch Note::NULL_PITCH = -1;

const Note Note::pitches[] = {
  // octave -1
  Note(0), Note(1), Note(2), Note(3), Note(4), Note(5),
  Note(6), Note(7), Note(8), Note(9), Note(10), Note(11),
  // octave 0
  Note(12), Note(13), Note(14), Note(15), Note(16), Note(17),
  Note(18), Note(19), Note(20), Note(21), Note(22), Note(23),
  // octave 1
  Note(24), Note(25), Note(26), Note(27), Note(28), Note(29),
  Note(30), Note(31), Note(32), Note(33), Note(34), Note(35),
  // octave 2
  Note(36), Note(37), Note(38), Note(39), Note(40), Note(41),
  Note(42), Note(43), Note(44), Note(45), Note(46), Note(47),
  // octave 3
  Note(48), Note(49), Note(50), Note(51), Note(52), Note(53),
  Note(54), Note(55), Note(56), Note(57), Note(58), Note(59),
  // octave 4
  Note(60), Note(61), Note(62), Note(63), Note(64), Note(65),
  Note(66), Note(67), Note(68), Note(69), Note(70), Note(71),
  // octave 5
  Note(72), Note(73), Note(74), Note(75), Note(76), Note(77),
  Note(78), Note(79), Note(80), Note(81), Note(82), Note(83),
  // octave 6
  Note(84), Note(85), Note(86), Note(87), Note(28), Note(89),
  Note(90), Note(91), Note(92), Note(93), Note(94), Note(95),
  // octave 7
  Note(96), Note(97), Note(98), Note(99), Note(100), Note(101),
  Note(102), Note(103), Note(104), Note(105), Note(106), Note(107),
  // octave 8
  Note(108), Note(109), Note(110), Note(111), Note(112), Note(113),
  Note(114), Note(115), Note(116), Note(117), Note(118), Note(119),
  // octave 9
  Note(120), Note(121), Note(122), Note(123), Note(124), Note(125),
  Note(126), Note(127)
};

constexpr Note::NOctave Note::get_octave(NPitch pitch) {
  return pitch / 12 - 1;
}

constexpr float Note::get_freq(NPitch pitch) {
  return 440.0 * pow(2, (float) (pitch - 69) / 12);
}

constexpr float Note::get_period(NPitch pitch) {
  return 1000000.0 / get_freq(pitch);
}

const char * Note::get_name(NPitch pitch) {
  static const char prefixes[] = "CDEFGAB";
  if (pitch < 21 || pitch > 127) {
    return nullptr;
  }

  char * res = new char[4] {};
  NOctave octave = get_octave(pitch);
  NPitch offset = pitch % 12;

  if (offset == 1 || offset == 3 || offset == 6 || offset == 8 || offset == 10) {
    offset--;  // for C#, D#, F#, G#, A#
    res[1] = '#';
    res[2] = '0' + octave;
  }
  else {
    res[1] = '0' + octave;
  }
  res[0] = prefixes[offset];

  return res;
}

constexpr Note::Note(NPitch pitch)
  : period(get_period(pitch)) {}
