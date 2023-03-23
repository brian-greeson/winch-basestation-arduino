void displayInit(){
   Heltec.display->init();
  // Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->clear();
  Heltec.display->drawString(0,0,"Winch Base");
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 30, "Waiting for data");
  Heltec.display->display();

}

void enableDisplay(){
  Heltec.display->displayOn();
  showDisplayTime = millis();
  winchState.showDispaly = true;
}

void disableDisplay(){
  winchState.showDispaly = false;
  Heltec.display->displayOff();
}

// TODO: clean this up and make it useful, for now it's just debug stuff
void displayStatus(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  uint8_t fontH = 12;
  Heltec.display->drawString(0, fontH * 0, "RSSI: " + String(LoRa.packetRssi(), DEC));  
  if(winchState.rewindFlag){
    Heltec.display->drawString(0, fontH * 1, "Rewind"); 
  } else {
    Heltec.display->drawString(0, fontH * 1, "Enable: " + String(winchState.enable, DEC)); 
  }
  Heltec.display->drawString(0, fontH * 2, "Throttle: "+ String(winchState.throttle, DEC));
  Heltec.display->drawString(0, fontH * 3, "Direction: "+ String(winchState.direction, DEC));
  Heltec.display->drawString(0, fontH * 4, "Line: "+ String(winchState.lineRemaining, DEC));
  Heltec.display->setTextAlignment(TEXT_ALIGN_RIGHT);
  Heltec.display->drawString(128, fontH * 0, "err:"+ String(winchState.error, DEC));
  // Heltec.display->drawString(128, fontH * 0, "db:"+ String(debugVal, DEC));


  // Heltec.display->drawString(128, fontH * 1, "Estop: " + String(winchState.es)); 
  // Heltec.display->drawString(128, fontH * 2, "B: " + String(digitalRead(HALL_B_PIN), DEC));
  // Heltec.display->drawString(128, fontH * 3, "C: " + String(digitalRead(HALL_C_PIN), DEC));
  Heltec.display->display();
}