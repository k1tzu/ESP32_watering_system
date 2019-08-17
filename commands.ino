void processIncomingCommand(String command) {
  Serial.printf("%s command received\n", command.c_str());
  if (command == "startPumping") {
    runWateringPump(WATERING_RUN_LONG_SECONDS, 0);
  } else if (command == "stopPumping") {
    stopWateringPump();
  } else if (command == "shutdown") {
    shutdownSystem();
  }
}

