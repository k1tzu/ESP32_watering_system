int pumpRunTimer = 0; //measure time pump is running
int pumpTimeout = 0; //

int runWateringPump(int timeToRunInSeconds, int pumpTimeoutCorrected) {
  if(pumpTimeoutCorrected <= 0) {
    pumpTimeout = pumpTimeoutCorrected * -1;
  }
  
  if (pumpRunTimer > timeToRunInSeconds) { //waiting for said time to stop the pump
    Serial.printf("pumpRunTimer %d > timeToRunInSeconds %d\n", pumpRunTimer, timeToRunInSeconds);
    stopWateringPump();
  } else if (!pumpTimeout && !pumpRunTimer) { //if there's no timeout we can run the pump
    Serial.printf("pumpTimeout %d\n", pumpTimeout);
    pumpRunTimer = 1; //starting pump run timer
    Serial.println("starting WateringPump");
    
    digitalWrite(RELAY, HIGH);
    
  } else if (pumpRunTimer) { //if there is timer - pump is running
    Serial.printf("pumpRunTimer %d\n", pumpRunTimer);
    pumpRunTimer++;
  } else if (pumpTimeout) { //if there is timout timer - we're waiting for pump to run
    Serial.printf("pumpTimeout %d\n", pumpTimeout);
    pumpTimeout++;
  }

  if (pumpTimeout > DELAY_BETWEEN_PUMPS_TIMER) {
    Serial.println("pumpTimeout > DELAY_BETWEEN_PUMPS_TIMER");
    pumpTimeout = 0; //if DELAY_BETWEEN_PUMPS_TIMER passed - reset timeout
  }

  return pumpRunTimer ? pumpRunTimer : (pumpTimeout * -1); //we return positive number if the pump
  //is running and negative if it's waiting
}

void stopWateringPump() {
  Serial.println("stopWateringPump");
  digitalWrite(RELAY, LOW);
  pumpRunTimer = 0; //reset timer
  pumpTimeout=1; //starting one hour delay
}

