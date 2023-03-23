#include "motor.h"
#include "utilities.h"

#define HALLS_COUNT_PER_REV 27
#define MAX_ROTATIONS 735
#define TOTAL_LINE 1000

struct MotorState motorState;

// Cummulative counter for the number of halls hit. 
volatile int32_t hallCount;
float lastRevolutionCheck;
uint64_t lastSpeedCheck;
uint64_t lastMotorUpdate;

void IRAM_ATTR hallIsr(){
  if(motorState.direction == 1){
    hallCount--;
  } else {
    hallCount++;
  }
}

void motorInit(){
  // Pin setup
  pinMode(HALL_PIN, INPUT);

  // Interrupt setup
  attachInterrupt(HALL_PIN, hallIsr, FALLING);
}

void getMotorUpdate(){
  winchState.direction = getDirection();
  // getRotations() must be called before speed and line Remaining
  motorState.rotations = getRotations();

  if(millis() - lastMotorUpdate > 50){
    winchState.speed = getSpeed();
    winchState.lineRemaining = getLineRemaining();
    lastMotorUpdate = millis();
  }
}

int16_t getLineRemaining(){
  // This is approximate, but close enough for us.
  // MAX_LINE / TOTAL_ROTATIONS
  return (int16_t)(motorState.rotations * 1.36054);
}

float getRotations(){
  float hallCountNow = hallCount;
  float rotations = hallCountNow / HALLS_COUNT_PER_REV;
  return rotations;
}

float getSpeed(){
  // S=[2∗PI∗(D/2)]∗[RPM/60]
  // Get Revolutions Per Second
  float elapsedTimeMs = millis() - lastSpeedCheck;
  float elapsedTimeSec = elapsedTimeMs / 1000.0;

  float revolutions = motorState.rotations - lastRevolutionCheck;

  lastRevolutionCheck = motorState.rotations;
  lastSpeedCheck = millis();
  
  float revolutionsPerSecond = revolutions / elapsedTimeSec;

 // Just using the average for now. If we ever want to use line speed for control we can calc the circumference again.
  float metersPerSecond = revolutionsPerSecond * 1.36054;

  return metersPerSecond;
}

uint8_t getDirection(){

  if(winchState.enable || winchState.rewindFlag){
    motorState.direction = payIn;
  } else {
    /* 
    We have to deal with the case of where to motor was paying in and then the power was cut, thus leaving the drum 
    to freewheel for a while. Could wait for speed to become very close to zero before switching directions, I mean, 
    it has to as some point switch direction. If we miss the switch our line remaining could be waaay off.
 */
    motorState.direction = payOut;
  }

  return motorState.direction;
}

void resetMotorRotations(){
  hallCount = 0;
  motorState.rotations = 0;
}

