#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <SPI.h>
#include <MFRC522.h>

// --- CREDENCIALES DE RED Y ADAFRUIT ---
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"

// --- VARIABLES DEL SENSOR Y RELOJ ---
#define PIN_RST 21
#define PIN_SDA 5
String miTarjeta = "11 22 33 44";
int intentosFallidos = 0;
int umbral = 3;
unsigned long tiempoTranscurrido = 10000;
unsigned long tiempoInicio = 0;
int modoPanico = false;

// --- OBJETOS DE CONEXIÓN ---
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish pub_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/control-tarjetas");

MFRC522 mfrc522(PIN_SDA, PIN_RST);
void setup() {
  Serial.begin(115200);
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
  // 1. Asegurar conexiones MQTT
  MQTT_connect();
  mqtt.processPackets(10);

  unsigned long tiempoActual = millis();
  if (modoPanico == true) {
    if (tiempoActual - tiempoInicio >= tiempoTranscurrido) {
      modoPanico = false;
      intentosFallidos = 0;
      Serial.println("Castigo terminado. Sistema desbloqueado.");
      pub_feed.publish((uint32_t)0);
    }
  } else {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      String tarjetaLeida = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        tarjetaLeida += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        tarjetaLeida += String(mfrc522.uid.uidByte[i], HEX);
      }
      tarjetaLeida.trim();
      tarjetaLeida.toUpperCase();
      Serial.print("Leída: ");
      Serial.println(tarjetaLeida);

      if (tarjetaLeida == miTarjeta) {
        Serial.println("Acceso concedido. Puerta abierta");
      } else {
        intentosFallidos++;
        Serial.print("Acceso denegado. Strikes: ");
        Serial.println(intentosFallidos);
      }

      if (intentosFallidos >= umbral) {
        modoPanico = true;
        tiempoInicio = millis();
        Serial.println("INTENTO BLOQUEADO");
        pub_feed.publish((uint32_t)1);
      }
      mfrc522.PICC_HaltA();
    }
  }
  // Mantener vivo el servidor de Adafruit
  if (!mqtt.ping()) {
    mqtt.disconnect();
  }

  // El respiro clave para que la placa no se trabe
  delay(50);
}

// Función para conectar y reconectar a Adafruit IO
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
    delay(5000);
  }
  Serial.println(F("¡MQTT Conectado!"));
}