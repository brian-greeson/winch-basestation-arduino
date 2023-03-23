#include "messages.h"
#define BANDWIDTH 41.7E3 // Possible 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3, 41.7E3, 62.5E3, 125E3, 250E3.
#define CODING_RATE 5
#define SPREADING_FACTOR 7

#define RX_BUFF_LENGTH 10
#define TX_BUFF_LENGTH 10
uint8_t rxBuff[RX_BUFF_LENGTH];
uint8_t txBuff[TX_BUFF_LENGTH];



void radioInit(){
    // Setup Lora
  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setCodingRate4(CODING_RATE);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  // Set recieve callback
  LoRa.onReceive(onReceive);
  // Set radio to recieve mode
  radioRecieve();
}

// Sets radio to recieve mode
void radioRecieve(){
  LoRa.receive();
}

// Sends a telemetry packet
void sendMessage()
{
  baseMessage.lineRemaining = winchState.lineRemaining;
  baseMessage.batteryLevel =  winchState.batteryLevel;
  baseMessage.error = winchState.error;
  baseMessage.speed = winchState.speed;
  baseMessage.direction = winchState.direction;
  baseMessage.time = millis();
  // Fill Buffer
  txBuff[0] = baseMessage.batteryLevel;            //Battery level. Not yet implimented
  txBuff[1] = (baseMessage.lineRemaining >> 8);   // First byte of line remaining
  txBuff[2] = (baseMessage.lineRemaining & 0xFF); // Second byte of line remaining
  txBuff[3] = baseMessage.id >> 8;                // Message id
  txBuff[4] = baseMessage.id & 0xFF;              // Message id
  txBuff[5] = baseMessage.speed;                  
  txBuff[6] = baseMessage.direction;


  // Send Buffer
  LoRa.beginPacket();
  for(int i = 0; i < TX_BUFF_LENGTH; i++){
    LoRa.write(txBuff[i]);
  }
  LoRa.endPacket(); // Sends packet
  radioRecieve();   // Put the radio back into recieve mode
}

// Recieve callback
void onReceive(int packetSize){
  lastPacketSize = packetSize;
  if (packetSize == 0) return;          // No packet, return
  
  // We got a telemetry request packet
  if(packetSize == 2){
    // Set the appropriate flags
    winchState.recievedTelemetryRequest = true;
    remoteMessage.time = millis();
  }else{
    // We got a normal packet
    for (int i =0; i < packetSize; i++) {
    rxBuff[i] = LoRa.read();
    }
    uint8_t flags = rxBuff[0];
    remoteMessage.enable = flags & 0x01;
    remoteMessage.rewind = flags & 0x02;
    remoteMessage.throttle = rxBuff[1];
    remoteMessage.id = (uint16_t)(rxBuff[2]) << 8;
    remoteMessage.id = remoteMessage.id | rxBuff[3];
  
    newMessage = true;
  }
    remoteMessage.time = millis();
}

// Increments the message id and sets all other variables to 0
void resetBaseMessage(){
  baseMessage.id = baseMessage.id + 1; // roll over in aproximiately 4.5hrs at 4Hz tx rate
  baseMessage.batteryLevel = 0;
  baseMessage.error = 0;
  baseMessage.lineRemaining = 0;
  baseMessage.time = 0;
}

void resetRemoteMessage(){
  remoteMessage.enable = 0;
  remoteMessage.error=0;
  remoteMessage.throttle = 0;
}