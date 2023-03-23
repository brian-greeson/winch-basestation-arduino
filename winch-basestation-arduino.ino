#include "heltec.h" 
#include "Wire.h"
#include "Adafruit_DS3502.h"
#include "buttons.h"
#include "messages.h"
#include "utilities.h"

#define STATE_REFRESH_RATE 500
#define SCREEN_REFRESH_RATE 200
// Kill the throttle if no message for 500ms
#define MESSAGE_TIMEOUT 1000
#define BAND    915E6  

#define RAMP_PERIOD 2000

struct RemoteMessage remoteMessage;

struct BaseMessage baseMessage;

// uint8_t debugVal =0;
struct WinchState{
  bool showDispaly;
  uint8_t towingFlag;
  uint8_t rewindFlag;
  uint8_t groundFlag;
  uint8_t enableCounter;
  float speed;
  uint8_t direction;
  uint8_t throttle;
  uint8_t enable;
  uint8_t error;
  int16_t lineRemaining;
  uint8_t batteryLevel;
  uint64_t lastRemoteMessageTime;
  uint8_t recievedTelemetryRequest;
  struct ButtonStates buttonStates;
} winchState;

Adafruit_DS3502 ds3502 = Adafruit_DS3502();

bool newMessage = false;
uint64_t showDisplayTime = 0;
uint64_t lastScreenRefresh = 0;
uint64_t lastStateUpdate = 0;
int lastPacketSize = 0;

void setup() { 
  winchState.groundFlag = true;
  winchState.towingFlag = winchState.rewindFlag = false;
  winchState.enable = 0;
  winchState.throttle = 0;
  winchState.showDispaly = true;
  showDisplayTime = millis();
  // Start the board
  Heltec.begin(true , true , false , true , BAND);
  // Heltec.begin(true , true , true , true , BAND); // Enable serial for debug
  Serial.begin(115200);
  // Setup Throttle
  throttleInit();

  // Setup Radio
  radioInit();

  // Setup Display
  displayInit();

  // Let things settle
  delay(100);

  // Setup buttons
  buttonsInit();

  // Setup Motor
  motorInit();

}


void loop() 
{

  // Buttons
  getButtons();
  if(winchState.buttonStates.buttonSelect){
    resetMotorRotations();
  }

    if(winchState.buttonStates.buttonUp){
      enableDisplay();
  }
  
  // Update Motor stats
  if(millis() - lastStateUpdate > 5){
  getMotorUpdate();
  lastStateUpdate = millis();
  }

  // Messages
  if(winchState.recievedTelemetryRequest){
    // Send Telemetry
    sendMessage();
    winchState.recievedTelemetryRequest = false;
    // Give us a little more time before hitting our timeout window becuase it can take a while to send the message.
    winchState.lastRemoteMessageTime = millis();
  }

  if(newMessage)
  {
    // We got a message
    // Clear the flag
    newMessage = false;

    // If we had a timeout, clear it because we got a new message
    if(winchState.error == ERR_MSG_TIMEOUT){
      winchState.error = 0;
    }
    // TODO: Validate message
    // Update state

    // Debounce the enable messages. 
    if(winchState.enable != remoteMessage.enable)
    {
      winchState.enableCounter++;
    }
    // Require two messages saying the enable has changed to commit changes
    if(winchState.enableCounter > 2){
      winchState.enable = remoteMessage.enable;
      winchState.enableCounter = 0;
    }
    // Always update..
    winchState.throttle = remoteMessage.throttle;
    winchState.rewindFlag = remoteMessage.rewind;
    winchState.lastRemoteMessageTime = remoteMessage.time;

    if(winchState.enable){
      // Limit max throttle based on line out.
      // if(winchState.lineRemaining < 200){
      //   if(winchState.throttle >50){
      //     winchState.throttle = 50;
      //   }
      // } else if(winchState.lineRemaining < 100){
      //   if(winchState.throttle > 30){
      //     winchState.throttle = 30;
      //   }
      // } else if(winchState.lineRemaining < 33){
      //   if(winchState.throttle > 20){
      //     winchState.throttle = 20;
      //   }
      // } else if(winchState.lineRemaining < 1){
      //   if(winchState.throttle > 0){
      //     winchState.throttle = 0;
      //   }
      // }
      setThrottle();
    } else if (winchState.rewindFlag){
      // overrides the throttle with rewind values
      setRewindThrottle();
      // Rewind will stop if there is a timeout error. 
      setThrottle();
    }else {
      ds3502.setWiper(0);
    }
  } else if(millis() - winchState.lastRemoteMessageTime > MESSAGE_TIMEOUT){
      eStop(ERR_MSG_TIMEOUT);
  }

  // Update screen if enabled
  if(winchState.showDispaly){
    if(millis() - lastScreenRefresh > SCREEN_REFRESH_RATE){
      lastScreenRefresh = millis();
      displayStatus();
    }
    if(millis() - showDisplayTime > 20000){
      disableDisplay();
    }
  }

}

void setThrottle(){
  if(winchState.error)
  {
    eStop(winchState.error);
  } else if(winchState.throttle != ds3502.getWiper()){
      ds3502.setWiper(winchState.throttle / 2);
    }
  
}
// Stop the winch now
void eStop(int reason){
  ds3502.setWiper(0);
  winchState.throttle = 0;
  winchState.enable = false;
  winchState.rewindFlag = false;
  winchState.error = reason;
}

void setRewindThrottle(){
  // Set throttle to maximum until the line get a little closer
  if(winchState.lineRemaining > 400){
    winchState.throttle = 255;
  } else if( winchState.lineRemaining > 240){
    // we are a little closer and slow down to reduce the chance of crashing the winch
    winchState.throttle = 150;
  } else if(winchState.lineRemaining > 134){
    winchState.throttle = 100;
  } else if (winchState.lineRemaining > 32){
    winchState.throttle = 35;
  } else if (winchState.lineRemaining > 1){
    winchState.throttle = 22;
  } else {
    winchState.throttle = 0;
  }

}
