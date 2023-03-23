#include "buttons.h"
#include "utilities.h"

#define DEBOUNCE_THRESHOLD 50

struct DebounceCounts debounceCounts;
struct ButtonStates   debounceStates;

void buttonsInit(){
  pinMode(BUTTON_UP_PIN, INPUT);
  pinMode(BUTTON_SELECT_PIN, INPUT);
  pinMode(BUTTON_DOWN_PIN, INPUT);
}

void getButtons(){
  winchState.buttonStates.buttonUp = getUpButton();
  winchState.buttonStates.buttonSelect= getSelectButton();
  winchState.buttonStates.buttonDown = getDownButton();
}

// Debounce buttons

uint8_t getDownButton(){
  uint64_t timeNow = millis();
  uint8_t ret = 0;
  bool btnState = !digitalRead(BUTTON_DOWN_PIN);

  if (btnState != debounceStates.buttonDown) {
    debounceCounts.buttonDownTime = timeNow;
    }
  if ((timeNow - debounceCounts.buttonDownTime) > DEBOUNCE_THRESHOLD) {
   ret = btnState;
   }
  debounceStates.buttonDown = btnState;

  return ret;
}

uint8_t getUpButton(){
  uint64_t timeNow = millis();
  uint8_t ret = 0;
  bool btnState = !digitalRead(BUTTON_UP_PIN);
  if (btnState != debounceStates.buttonUp) {
    debounceCounts.buttonUpTime = timeNow;
    }
  if ((timeNow - debounceCounts.buttonUpTime) > DEBOUNCE_THRESHOLD) {
   ret = btnState;
   }
  debounceStates.buttonUp = btnState;

  return ret;
}

uint8_t getSelectButton(){
  uint64_t timeNow = millis();
  uint8_t ret = 0;
  bool btnState = !digitalRead(BUTTON_SELECT_PIN);
  if (btnState != debounceStates.buttonSelect) {
    debounceCounts.buttonSelectTime = timeNow;
    }
  if ((timeNow - debounceCounts.buttonSelectTime) > DEBOUNCE_THRESHOLD) {
   ret = btnState;
   }
  debounceStates.buttonSelect = btnState;

  return ret;
}


