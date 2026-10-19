void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(1000);
  Serial.println("=======================");
  Serial.println(" Alejandra Quezada - 00066224");
  Serial.println("=======================");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("ESP32 funcionando correctamente -Tiempo activa: ");
  Serial.print(millis() / 1000.0);
  Serial.println(" segundos");

  delay(2000);
}
