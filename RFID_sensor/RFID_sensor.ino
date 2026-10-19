#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 22
#define SS_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Acerca una tarjeta RFID al lector...");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!rfid.PICC_IsNewCardPresent()){
    return;
  }

  if(!rfid.PICC_ReadCardSerial()){
    return;
  }

  Serial.print("UID de la tarjeta: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
