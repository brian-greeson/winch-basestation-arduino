#include <sys/_stdint.h>
#ifndef MESSAGES_H
#define MESSAGES_H
struct RemoteMessage {
  uint8_t throttle;
  uint8_t enable;
  uint8_t error;
  uint8_t rewind;
  uint16_t id;
  uint64_t time;
};

struct BaseMessage {
  int8_t speed;
  uint8_t direction;
  int16_t lineRemaining;
  uint8_t batteryLevel;
  uint8_t error;
  uint16_t id;
  uint64_t time;
};
#endif