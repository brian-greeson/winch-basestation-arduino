void throttleInit(){
   if (!ds3502.begin()) 
  {
    winchState.error = ERR_THROTTLE_CTRL;
  }else{
  ds3502.setWiperDefault(0);
  ds3502.setWiper(0);
  }
}