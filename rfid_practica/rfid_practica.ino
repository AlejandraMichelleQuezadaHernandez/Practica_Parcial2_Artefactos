#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <SPI.h>
#include <MFRC522.h>

// Credenciales de tu red Wi-Fi
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""

// Credenciales de Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"
#define RST_PIN 22
#define SDA_PIN 5

// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
uint32_t contador = 0; 
// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_RFID = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/control-rfid");

MFRC522 mfrc522(SDA_PIN, RST_PIN);
void setup() {
  Serial.begin(115200);
  delay(10);
  SPI.begin();
  mfrc522.PCD_Init();
  

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();
  mqtt.processPackets(10);
if(!mfrc522.PICC_IsNewCardPresent()){
  return;
}

if(!mfrc522.PICC_ReadCardSerial()){
  return;
}

contador++;
  Serial.print("Paso de tarjeta: ");
  Serial.println(contador);

Serial.print("UID de la tarjeta: ");
for(byte i = 0; i < mfrc522.uid.size; i++){
  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  Serial.print(mfrc522.uid.uidByte[i], HEX);
}
Serial.println();

if(pub_RFID.publish(contador)){
  Serial.println("Datos publicados correctamente");
}
mfrc522.PICC_HaltA();
mfrc522.PCD_StopCrypto1();
 
delay(2000);


  // Mantener vivo el servidor (Ping)
  if (!mqtt.ping()) {
    mqtt.disconnect();
  }
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Conectando a MQTT... "));
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println(F("Reintentando conexión en 5 segundos..."));
    mqtt.disconnect();
    delay(5000); // Este delay solo ocurre si se cae el servidor
  }
  Serial.println(F("¡MQTT Conectado!"));
}
