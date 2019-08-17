#ifdef REMOTE_SERVER
int failedConnection = 0;
bool connectToWifi() {
  // wait for WiFi connection
  //  WiFi.softAPdisconnect();
  //  WiFi.disconnect();
  //  WiFi.mode(WIFI_OFF);
  //  WiFi.mode(WIFI_STA);
  delay(100);

  WiFiMulti.addAP(ssid, password);

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    Serial.println("WiFi connected");
    Serial.print("WiFi IP:"); Serial.println(WiFi.localIP());
    failedConnection = 0;
    return true;
  } else {
    Serial.println("WiFi connection failed");
    if (failedConnection++ > 999) {
      failedConnection = 0;
      Serial.print("Quitting from wifi connection loop\n");
//      ESP.restart();
      return false;
    }
    delay(1000);
    return connectToWifi();
  }
}

void checkIfWiFiClientConnected() {
  if (wifiConnected) {
    if (wifiAPserverRunning) {
      wifiAPserverRunning = false;
      WiFi.mode(WIFI_OFF);
    }

    int status = WiFiMulti.run();
    if (status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {
      Serial.println("WiFi connection lost. Reconnecting.");
      if (greenLedState) {
        greenLedState = false;
        digitalWrite(GREEN_LED, HIGH);
      }
      //      ledToggle(RED_LED); //blinking
      //      delay(1000);
    } else if (!greenLedState) {
      Serial.println("WiFi connection established.");
      greenLedState = true;
      digitalWrite(GREEN_LED, LOW);
    }
  }
    //    ledToggle(RED_LED); //blinking
}
#endif
