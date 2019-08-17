void loadCredentials() {
  EEPROM.begin(512);
  char ok[2 + 1];
  int i = 0;

  Serial.println("\nEEPROM reading ssid:");
  for (i = 0; i < sizeof(ssid); i++) {
    if (EEPROM.read(i) == '|') break;
    ssid[i] = EEPROM.read(i);
    Serial.printf("%d: '%c'", i, EEPROM.read(i));
  }

  if (EEPROM.read(i) != '|') {
    Serial.println("\nNo credentials");
    ssid[0] = 0;
    password[0] = 0;
    EEPROM.end();
    return;
  }

  ssid[i] = '\0';

  Serial.printf("\nstrlen(ssid): %d\n", strlen(ssid));
  Serial.println("\nEEPROM reading password:");
  int startingPosition = strlen(ssid) + 1;

  for (i = 0; i < sizeof(password); i++) {
    if (EEPROM.read(i + startingPosition) == '|') break;
    password[i] = EEPROM.read(i + startingPosition);
    Serial.printf("%d: '%c'", i, EEPROM.read(i + startingPosition));
  }

  i += startingPosition;

  if (EEPROM.read(i) != '|' || EEPROM.read(i + 1) != 'o' || EEPROM.read(i + 2) != 'k') {
    Serial.println("\nNo credentials");
    ssid[0] = 0;
    password[0] = 0;
    EEPROM.end();
    return;
  }

  Serial.println("\nRecovered credentials:");
  Serial.println(ssid);
  //  Serial.println(strlen(password) > 0 ? "********" : "<no password>");
  Serial.println(password);
  EEPROM.end();
}

void saveCredentials() {
  Serial.println("Saving credentials:");
  Serial.println(ssid);
  Serial.println(strlen(ssid));
  Serial.println(password);
  Serial.println(strlen(password));
  EEPROM.begin(512);
  Serial.print("\nwriting EEPROM ssid: ");
  for (int i = 0; i < strlen(ssid); i++)
  {
    EEPROM.write(i, ssid[i]);
    //    Serial.print(ssid[i]);
  }
  //  Serial.print("\nwriting EEPROM delimiter");
  EEPROM.write(strlen(ssid), '|');

  Serial.print("\nwriting EEPROM pass: ");
  for (int i = 0; i < strlen(password); i++)
  {
    EEPROM.write(strlen(ssid) + 1 + i, password[i]);
    //    Serial.print(password[i]);
  }
  //  Serial.print("\nwriting EEPROM delimiter");
  int ending = strlen(ssid) + strlen(password) + 1;
  EEPROM.write(ending, '|');
  EEPROM.write(ending + 1, 'o');
  EEPROM.write(ending + 2, 'k');

  //  EEPROM.put(0, ssid);
  //  EEPROM.put(0+sizeof(ssid), password);
  //  char ok[2+1] = "OK";
  //  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();

  //  Serial.print("\nreading EEPROM: ");
  //  for (int i = 0; i < strlen(ssid) + strlen(password) + 2; i++)
  //  {
  //    Serial.printf("%d: '%c'", i, EEPROM.read(i));
  //  }
  EEPROM.end();
  ESP.restart();
}

