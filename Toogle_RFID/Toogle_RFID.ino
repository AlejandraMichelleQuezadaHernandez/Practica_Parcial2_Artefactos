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
#define AIO_KEY         "aio_Rtqi86HI5kOaHQHhYa9Vnl7Y7Ner"

#define PIN_RST 22
#define PIN_SDA 5

MFRC522 rfid(PIN_SDA, PIN_RST);
// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
unsigned long tiempoAnterior = 0;
const long intervalo = 5000;
bool sistemaBloqueado = false;
String uidString = "";

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_registro = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/registro-ingreso");
Adafruit_MQTT_Subscribe sub_control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bloqueo-sistema");

void manejoSistema(char *data, uint16_t longitud){
  if(strcmp(data, "ON") == 0){
    sistemaBloqueado = true;
  }else{
    sistemaBloqueado = false;
  }
}
void setup() {
  Serial.begin(115200);
  delay(10);

  SPI.begin();
  rfid.PCD_Init();

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  sub_control.setCallback(manejoSistema);
  mqtt.subscribe(&sub_control);
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();

  // --- RETRASO Y ESCUCHA SIMULTÁNEA ---
  // En lugar de delay(5000), usamos processPackets(5000).
  // Esto hace una pausa de 5 segundos, pero procesa CUALQUIER mensaje que llegue en ese tiempo.
  mqtt.processPackets(10);

  if(sistemaBloqueado == false){
  if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
    uidString = "";
    for(byte i = 0; i < rfid.uid.size; i++){
      uidString += String(rfid.uid.uidByte[i], HEX);

    }
    Serial.print("Tarjeta detectada. UID: ");
    Serial.println(uidString);

    if(pub_registro.publish(uidString.c_str())){
      Serial.println("Ingreso publicado con éxito.");

    }else{
      Serial.println("Error al publicar.");

    }

    rfid.PICC_HaltA();
    delay(1000);
  }
  }
  

  // Mantener vivo el servidor (Ping)
  if (!mqtt.ping()) {
    mqtt.disconnect();
  }

  delay(50);
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
