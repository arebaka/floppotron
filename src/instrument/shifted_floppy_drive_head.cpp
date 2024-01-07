#include "shifted_floppy_drive_head.h"

using instrument::ShiftedFloppyDriveHead;
using util::ShiftRegister;

void ShiftedFloppyDriveHead::write_direction() {
  shift_register.write(this, FloppyDriveHead::direction_pin, FloppyDriveHead::direction);
  shift_register.sync();
}

void ShiftedFloppyDriveHead::write_phase() {
  shift_register.write(this, step_pin, phase);
  shift_register.tick(this);
}

ShiftedFloppyDriveHead::ShiftedFloppyDriveHead(ShiftRegister & shift_register, NPin step_pin, NPin direction_pin, NPosition n_positions)
: FloppyDriveHead(step_pin, direction_pin, n_positions), shift_register(shift_register) {
  shift_register.attach_instrument(this);
}

void ShiftedFloppyDriveHead::setup() {
  reset();
  // delay(TIME_TO_WAIT_AFTER_SETUP);  // wait for safety
}

void ShiftedFloppyDriveHead::tick(Time time) {
  FloppyDriveHead::tick(time);
  shift_register.tick(this);
}
