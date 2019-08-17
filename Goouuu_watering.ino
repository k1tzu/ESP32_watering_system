#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
//deep sleep include
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cont.h"

#include "config.h"

}

char  password[] = WIFI_PASSWORD;
char  ssid[] = WIFI_SSID;
String defaultAPpassword = AP_PASSWORD;

#define WATERING_RUN_LONG_SECONDS 120
#define WATERING_RUN_SHORT_SECONDS 60
#define DELAY_BETWEEN_PUMPS_TIMER 3600
#define LED 4
#define RED_LED 12
#define GREEN_LED 13
#define BLUE_LED 14
#define RELAY 16
#define SOILSENSORPOWER 14
#define DRY_SOIL 300
#define WET_SOIL 1500
#define LOCAL_UDP_PORT 64399  //high enough to pass through ISP NAT untouched

//forward declaration needed if we use separate files in project
void startWiFiAP(String mac);
bool connectToWifi();
bool connectUDP();
void startHttpServer();
String checkIfThereIsUDPmessage();
void checkIfThereIsHTTPclient();
void checkIfWiFiClientConnected();
int checkSensorValue();
void processIncomingCommand(String UDPmessage);
void stopWateringPump();
int runWateringPump(int timeToRunInSeconds, int pumpTimeoutCorrected = 0);
//////////////////////////////////////////////////////

ESP8266WiFiMulti WiFiMulti;

//char ssid[32], password[32];

bool wifiConnected = false;

bool wifiAPserverRunning = false;
bool sensorRunning = false;
bool greenLedState = false;
bool state = false;
String mac;
int sensorValue = 2000;

void ledToggle(int ledPin) {                                                          //toggle led
  digitalWrite(ledPin, (!state) ? LOW : HIGH);                                 //!state because otherwise the led would be inverted
  //  USE_SERIAL.printf("LED TOGGLE %d\n", state);
  state = !state;                                                           //toggle the state function to switch the led state
}

void ledLoop(const byte nbTics = 1, int ledPin = 4) {
  for (int i = 0; i < nbTics * 2; i++) {
    ledToggle(ledPin);
    delay(500);
    ledToggle(ledPin);
  }
}

void shutdownSystem() {
  digitalWrite(RELAY, LOW);
  digitalWrite(SOILSENSORPOWER, LOW);
  system_deep_sleep_set_option(2);
  Serial.println("Shutting down");
  system_deep_sleep(0);
}

void setup() {
//  password[0] = 0;
//  ssid[0] = 0;
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, HIGH);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  pinMode(RELAY, OUTPUT);

  delay(1000);

  Serial.begin(115200);

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  mac = String(WiFi.macAddress());
  mac.replace(":", "");
  mac.remove(0, 4);

  //    runWifiServer(mac);
  //    return;
#ifdef REMOTE_SERVER
  //loadCredentials();
  Serial.println("loadCredentials ended");
#endif

#ifdef REMOTE_SERVER
  if (!strlen(password)) {
#endif
    Serial.println("Starting wifi AP");
    startWiFiAP(mac);
#ifdef REMOTE_SERVER
  } else {
    WiFiMulti.addAP(ssid, password);
    wifiConnected = connectToWifi(); //connect to WIFI we filled in to HTTP server

    if (wifiConnected) {
      greenLedState = true;
      digitalWrite(GREEN_LED, LOW);
      bool udpConnected = connectUDP(); //send actions to server and recieve commands just in case
      if (udpConnected) {
        Serial.println("UDP connected");
      } else {
        Serial.print("Can't establish UDP");
      }
    } else {
      Serial.print("Can't establish WiFI - starting http server and access point");
      startWiFiAP(mac);
    }
  }
#endif
  startHttpServer();

  sensorRunning = true;

  sensorValue = checkSensorValue(); //we read current sensor value only if pump is not running...  
}

int pumpRunningSeconds = 0; //it's negative if we have timeout waiting
int runCounter=0;

void loop() {
  if (pumpRunningSeconds <= 0) {
//    if(runCounter++ > 600){ //revery hour
      runCounter = 0;
      sensorValue = checkSensorValue(); //we read current sensor value only if pump is not running...
//    }
  }

  if (sensorValue < DRY_SOIL) {
    Serial.println("Soil is dry");
    pumpRunningSeconds = runWateringPump(WATERING_RUN_LONG_SECONDS, pumpRunningSeconds);
  } else if (sensorValue < WET_SOIL) {
    Serial.println("Soil is almost wet");
    pumpRunningSeconds = runWateringPump(WATERING_RUN_SHORT_SECONDS, pumpRunningSeconds);
  } else if (sensorValue > WET_SOIL) {
    Serial.println("Soil is wet");
    if (pumpRunningSeconds) {
      //pump is running by FORCE
      pumpRunningSeconds = runWateringPump(WATERING_RUN_LONG_SECONDS, pumpRunningSeconds);
    } else {
      pumpRunningSeconds--;
    }
  }

  checkIfThereIsHTTPclient(mac); //If somebody connects to http we must show him something

  String pumpState = pumpRunningSeconds >= 0 ? "ON" : "OFF";
  printf("{\"sensor\":\"%s\",\"mositure\":%d,"\
  "\"drySoilValue\": %d,\"wetSoilValue\":%d,\"pumpstate\":%s"\
  ",\"pumpTimer\":%d}\n", mac.c_str(), sensorValue, DRY_SOIL,
  WET_SOIL, pumpState.c_str(), pumpRunningSeconds);

  updateDataInIndex(sensorValue, DRY_SOIL, WET_SOIL, pumpState, pumpRunningSeconds);

#ifdef REMOTE_SERVER
  if (wifiConnected) {
    checkIfWiFiClientConnected(); //If WiFi is available check it's status and reconnect if needed
    String UDPmessage = checkIfThereIsUDPmessage();
    if (UDPmessage.length() > 1) processIncomingCommand(UDPmessage);
    sendStatToUDP(mac, sensorValue, pumpState, pumpRunningSeconds);
  }
#endif

  delay(1000); //this is a 1 second. Watering pump runs with the assumption it's 1 second. Do not change.
}
