#ifndef BUTTONS_H
#define BUTTONS_H

struct ButtonStates{
  bool buttonUp;
  bool buttonSelect;
  bool buttonDown;
};

struct DebounceCounts{
  uint64_t buttonUpTime;
  uint64_t buttonSelectTime;
  uint64_t buttonDownTime;
} ;

#endif