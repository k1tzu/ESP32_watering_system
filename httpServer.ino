#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

String firstPartOfIndex("<!doctype html><meta content=10 http-equiv=refresh><html xmlns=http://www.w3.org/1999/xhtml><meta content=\"text/html; charset=UTF-8\"http-equiv=Content-Type><meta content=\"width=device-width,initial-scale=1,user-scalable=no,minimum-scale=1,maximum-scale=1\"name=viewport><title>Watering state</title><style>body{background:#fffff;font-family:\"Lucida Grande\",Tahoma,Arial,Verdana,sans-serif;font-size:small;margin:8px 0 16px;text-align:center}#form_container{border-width:0;outline:0;border-radius:2px;box-shadow:0 1px 4px rgba(0,0,0,.6);background-color:#2ecc71;transition:background-color .3s;background:#fff;border:1px solid #cacc;margin:0 auto;text-align:left;width:auto}h1{background-color:#dedede;margin:0;min-height:0;padding:0;text-decoration:none;text-indent:-8000px}h1 a{display:block;height:100%;min-height:1em;overflow:hidden}.appnitro{margin:1em;padding:.5em;font-family:Lato,sans-serif;font-weight:700;margin:.5em;float:left;font-family:Lucida Grande,Tahoma,Arial,Verdana,sans-serif;font-size:small;border-bottom:1px solid #e7eef1;overflow:hidden;border-width:0;outline:0;border-radius:2px;box-shadow:0 1px 4px rgba(0,0,0,.6)}@media screen and (max-width:1366px){.appnitro{width:30%}}@media screen and (max-width:400px){.appnitro{width:90%}}form ul{font-size:100%;list-style-type:none;margin:0;padding:0;width:100%}form li{display:block;margin:0;padding:4px 5px 2px 9px;position:relative}form li:after{clear:both;content:\".\";display:block;height:0;visibility:hidden}* html form li{height:1%}* html form li div{display:inline-block}form li div{color:#444;margin:0 4px 0 0;padding:0 0 8px}form li div.left{display:inline;float:left;width:48%}form li div.right{display:inline;float:right;width:48%}form li div.left .medium{width:100%}form li div.right .medium{width:100%}.clear{clear:both}form li div label{clear:both;color:#444;display:block;font-size:9px;line-height:9px;margin:0;padding-top:3px}.form_description{border-bottom:1px dotted #ccc;clear:both;display:inline-block;margin:0 0 1em}.form_description[class]{display:block}.form_description h2{clear:left;font-size:160%;font-weight:400;margin:0 0 3px}.form_description p{font-size:95%;line-height:130%;margin:0 0 12px}form hr{display:none}form li.section_break{border-top:1px dotted #ccc;margin-top:9px;padding-bottom:0;padding-left:9px;padding-top:13px;width:97%!important}form ul li.first{border-top:none!important;margin-top:0!important;padding-top:0!important}form .section_break h3{font-size:110%;font-weight:400;line-height:130%;margin:0 0 2px}form .section_break p{font-size:85%;margin:0 0 10px}label.description{border:none;color:#222;display:block;font-size:95%;font-weight:700;line-height:150%;padding:0 0 1px}input.text{border-bottom:1px solid #ddd;border-left:1px solid #c3c3c3;border-right:1px solid #c3c3c3;border-top:1px solid #7c7c7c;color:#333;font-size:100%;margin:0;padding:2px 0}input.medium{width:50%}.btn{font-family:Lato,sans-serif;font-size:12px;font-weight:700;text-transform:uppercase;border-bottom:1px solid #e7eef1;width:80%;height:3em!important;margin:1em!important;position:relative;display:block;margin:30px auto;overflow:hidden;border-width:0;outline:0;border-radius:2px;box-shadow:0 1px 4px rgba(0,0,0,.6);background-color:#42a5f5;color:#ecf0f1;transition:background-color .3s}.btn:focus,.btn:hover{background-color:#3484c4}.btn>*{position:relative}.btn span{display:block;padding:12px 24px}.btn:before{content:\"\";position:absolute;top:50%;left:50%;display:block;width:0;padding-top:0;border-radius:100%;background-color:rgba(236,240,241,.3);-webkit-transform:translate(-50%,-50%);-moz-transform:translate(-50%,-50%);-ms-transform:translate(-50%,-50%);-o-transform:translate(-50%,-50%);transform:translate(-50%,-50%)}.btn:active:before{width:120%;padding-top:120%;transition:width .2s ease-out,padding-top .2s ease-out}</style><body id=main_body><div id=form_container><h1><a>Sensor State</a></h1><form action=/submit class=appnitro method=post id=form><div class=form_description><h2>Local System ");
String sensorSerialPart("");
String secondPartOfIndex("</h2><p></div><ul><li><label class=description for=soilState>Soil State</label><div id=soilState style=text-align:center;color:#ecf0f1;");
String soilStatePart("background-color:#f0f8ff>NONE");
String thirdPartOfIndex("</div><label class=description for=pumpState>Pump State</label><div id=pumpState style=text-align:center;color:#ecf0f1;");
String pumpStatePart("background-color:#f0f8ff>NONE");
String fourthPartOfIndex("</div><li><button class=btn id=startPumping name=startPumping type=submit>Start pumping</button> <button class=btn id=stopPumping name=stopPumping type=submit>Stop pumping</button> <button class=btn id=shutdown name=shutdown type=submit onclick='return confirm(\"THIS WILL TURN OFF SYSTEM FOREVER\")'>Shutdown system</button></ul></form>");
#ifdef REMOTE_SERVER
String wifiFormPart("<form action=/submit class=appnitro method=post><div class=form_description><h2>WIFI Connection Settings</h2><p></div><ul><li id=li_1><label class=description for=ssidName>WIFI SSID Name</label><div><input class=\"element medium text\"id=ssidName maxlength=255 name=ssidName required></div><li id=li_2><label class=description for=wifiPassword>Password</label><div><input class=\"element medium text\"id=wifiPassword maxlength=255 name=wifiPassword required></div><li class=buttons><button class=btn id=saveForm name=submit type=submit value=Submit>Save</button></ul></form>");
#else
String wifiFormPart("");
#endif
String lastPartOfIndex("</div>");

void handleRoot() {
  server.send(200, "text/html", firstPartOfIndex + sensorSerialPart + secondPartOfIndex + soilStatePart + thirdPartOfIndex + pumpStatePart + fourthPartOfIndex + wifiFormPart + lastPartOfIndex);
}

void startHttpServer() {
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();

  Serial.println("HTTP server started");
}

void  updateDataInIndex(int sensorValue, int dry_soil, int wet_soil, String pumpState, int pumpRunningSeconds) {
  soilStatePart = updateSoilState(sensorValue, dry_soil, wet_soil);
  pumpStatePart = updatePumpState(pumpState, pumpRunningSeconds);
}
  
String updateSoilState(int sensorValue, int dry_soil, int wet_soil) {
  if (sensorValue < dry_soil) {
    return "background-color:#ffca28;>DRY";
  } else if (sensorValue < wet_soil) {
    return "background-color:#8dc9f9;>MOIST";
  } else if (sensorValue >= wet_soil) {
    return "background-color:#3b94dc;>WET";
  }
}

String updatePumpState(String pumpState, int pumpRunningSeconds) {
  if (pumpState == "ON") {
    String pumpRunningTime = secondsToString(pumpRunningSeconds);
    return "background-color:#ef5350;>ON "+pumpRunningTime; 
  } else if (pumpState == "OFF") {
    pumpRunningSeconds *= -1;
    String pumpRunningTime = secondsToString(pumpRunningSeconds);
    Serial.print("pumpRunningTime:");
    Serial.println(pumpRunningTime);
    return "background-color:#7986cb;>OFF "+pumpRunningTime;
  }  
}

String secondsToString(int seconds){
  long days = seconds / (3600 * 24);
  String humanTime = days ? String(days) + "d " : "0d ";
  seconds -= (days * 3600 * 24);
  long hours = seconds / 3600;
  humanTime += hours ? String(hours) + "h " : "0h ";
  seconds -= (hours * 3600);
  long minutes = seconds / 60;
  humanTime += minutes ? String(minutes) + "m ": "0m ";
  seconds -= (minutes * 60);
  humanTime += seconds ? String(seconds) + "s ": "0s ";
  return humanTime;
}

void handleSubmit() {
  int gotCredentials = 0;
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "ssidName") {
        Serial.printf("SSID: %s\n", server.arg(i).c_str());
        server.arg(i).toCharArray(ssid, 32);
        ssid[server.arg(i).length() + 1] = '\0';
        gotCredentials++;
        continue;
      }

      if (server.argName(i) == "wifiPassword") {
        Serial.printf("Password: %s\n", server.arg(i).c_str());
        server.arg(i).toCharArray(password, 32);
        password[server.arg(i).length() + 1] = '\0';
        gotCredentials++;
        continue;
      }
      processIncomingCommand(server.argName(i));
    }
  }

  if (gotCredentials == 2) {
    saveCredentials();
  }

  server.send(200, "text/html", "<!doctype html><html lang=en-US>\n<meta charset=UTF-8><meta content=\"2; url=/\"http-equiv=refresh><script>window.setTimeout(window.history.back(),3000)</script><title>Page Redirection</title>\"Message Sent\"");
}

void checkIfThereIsHTTPclient(String serial) {
  sensorSerialPart = serial;
  server.handleClient();
}


