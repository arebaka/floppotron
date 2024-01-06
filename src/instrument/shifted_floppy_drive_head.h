#pragma once
#include "floppy_drive_head.h"
#include "../util/shift_register.h"

namespace instrument {
class ShiftedFloppyDriveHead : public FloppyDriveHead {
protected:
  util::ShiftRegister & shift_register;

  void write_direction();
  void write_phase();

public:
  ShiftedFloppyDriveHead(
    util::ShiftRegister & shift_register, NPin step_pin, NPin direction_pin,
    NPosition n_positions = FloppyDriveHead::DEFAULT_N_POSITIONS
  );

  void setup() override;
};
}
