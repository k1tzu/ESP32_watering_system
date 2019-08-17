
void startWiFiAP(String mac) {
  String defaultSsid = "sensor" + mac;

  //  WiFi.disconnect();
  //  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_AP);
  delay(100);

  WiFi.softAP(defaultSsid.c_str(), defaultAPpassword.c_str());

  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(myIP);

  wifiAPserverRunning = true;
}
