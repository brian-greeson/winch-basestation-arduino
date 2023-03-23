
#ifndef MOTOR_H
#define MOTOR_H
enum MotorDirection{
  dir_error,
  payIn,
  payOut
};

enum HallSensors{
  hallA,
  hallB,
  hallC
};

struct MotorState{
  MotorDirection direction;
  // Cummulative rotations the drum has experienced:
  // If the drum rotates 5 times fwd, then 2 time in rev then rotations = 3
  float rotations;
  uint64_t lastSpeedCheck;
};

#endif