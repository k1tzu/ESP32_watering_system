
ADC_MODE(ADC_VCC);

int checkSensorValue() {
  pinMode(SOILSENSORPOWER, HIGH);
  delay(100);
  int voltage = ESP.getVcc();
  pinMode(SOILSENSORPOWER, LOW);
  Serial.print("Sensor value: ");
  Serial.println(voltage);
  return voltage;
}

