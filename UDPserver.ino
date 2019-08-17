#ifdef REMOTE_SERVER
// UDP variables
WiFiUDP UDP;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged"; //a string to send back

bool connectUDP() {
  if (UDP.begin(LOCAL_UDP_PORT) == 1) {
    Serial.println("Connection successful");
    return true;
  }
  else {
    Serial.println("Connection failed");
    return false;
  }
}

String processUDPpacket(int packetSize) {
  Serial.println("");
  Serial.print("Received packet of size ");
  Serial.println(packetSize);
  Serial.print("From ");
  IPAddress remote = UDP.remoteIP();
  for (int i = 0; i < 4; i++)
  {
    Serial.print(remote[i], DEC);
    if (i < 3)
    {
      Serial.print(".");
    }
  }
  Serial.print(", port ");
  Serial.println(UDP.remotePort());

  // let's clear the buffer first
  memset(packetBuffer, 0, sizeof(packetBuffer));
  // read the packet into packetBufffer
  UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

  packetBuffer[packetSize] = 0;
  Serial.printf("Contents: %s\n", packetBuffer);
  //  for(int i=0; i< packetSize;i++){
  //    char value = packetBuffer[i];
  //    Serial.printf("[%d]=%s ", i, value);
  //  }
  Serial.println();

  // send a reply, to the IP address and port that sent us the packet we received
  UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
  UDP.write(ReplyBuffer);
  UDP.endPacket();

  if (packetBuffer) {
    return String(packetBuffer);
  } else {
    return String();
  }
}

String checkIfThereIsUDPmessage() {
  String UDPmessage;
  int packetSize = UDP.parsePacket();

  if (packetSize)
  {
    UDPmessage = processUDPpacket(packetSize);
  }
  
  return UDPmessage;
}

void sendStatToUDP(String sensorID, int sensorValue, String pumpState, int pumpRunningSeconds) {
  UDP.beginPacket(REMOTE_SERVER, LOCAL_UDP_PORT);
//  UDP.printf("{\"sensor\":\"%s\",\"mositure\":%d,\"pumpstate\":\"%s\",\"pumpTimer\":%d}",sensorID.c_str(), sensorValue, pumpState.c_str(), pumpRunningSeconds);
  UDP.printf("{\"sensor\":\"%s\",\"mositure\":%d,\"drySoilValue\": %d,\"wetSoilValue\":%d,\"pumpstate\":%s,\"pumpTimer\":%d}",sensorID.c_str(), sensorValue, DRY_SOIL, WET_SOIL, pumpState.c_str(), pumpRunningSeconds);
  UDP.endPacket();  
}
#endif

