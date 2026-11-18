#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ---------------------- CONFIGURACIÓN WI-FI ----------------------
#define WLAN_SSID   "redwifi"
#define WLAN_PASS   "wifipassword"

// ---------------------- CONFIGURACIÓN ADAFRUIT IO ----------------------
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "useradafruit"   
#define AIO_KEY         "key"             

// ---------------------- PINES ----------------------
const int sensorIR = 2;

// ---------------------- CLIENTE MQTT Y FEEDS ----------------------
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feed del contador en Adafruit IO
Adafruit_MQTT_Publish feedContador = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/contador_personas");

// ---------------------- VARIABLES DE CONTROL ----------------------
int contador = 0;
int estadoAnterior = HIGH;
unsigned long ultimoPing = 0;
unsigned long ultimoMuestreo = 0;
const int intervaloMuestreo = 300; // Equivalente a tu delay(300) sin bloquear el CPU

// ---------------------- PROTOTIPOS ----------------------
void conectarWiFi();
void conectarMQTT();

// =====================================================================
void setup() {
  Serial.begin(115200); // 115200 baudios recomendado para ESP32
  pinMode(sensorIR, INPUT);

  conectarWiFi();
}

// =====================================================================
void loop() {
  conectarMQTT();   

  // Lectura periódica cada 300 ms (reemplaza delay(300) para no congelar MQTT)
  if (millis() - ultimoMuestreo >= intervaloMuestreo) {
    ultimoMuestreo = millis();

    int estado = digitalRead(sensorIR);

    // Muestra en consola el estado tal como lo tenías
    Serial.print("Estado del Sensor: ");
    Serial.println(estado);

    // Detección de flanco: si pasa de HIGH (libre) a LOW (obstáculo detectado)
    if (estado == LOW && estadoAnterior == HIGH) {
      contador++;
      Serial.print("--> Persona detectada. Conteo acumulado: ");
      Serial.println(contador);

      // Enviar el nuevo valor a Adafruit IO
      if (!feedContador.publish((int32_t)contador)) {
        Serial.println("Error al publicar en Adafruit");
      }
    }

    // Actualizar estado para la siguiente comparación
    estadoAnterior = estado;
  }

  // Ping de mantenimiento de sesión MQTT cada 30 segundos
  if (millis() - ultimoPing >= 30000) {
    ultimoPing = millis();
    mqtt.ping();
  }
}

// =====================================================================
// CONEXIONES
// =====================================================================
void conectarWiFi() {
  Serial.print("Conectando a "); Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\nWiFi conectado exitosamente.");
}

void conectarMQTT() {
  if (mqtt.connected()) return;
  Serial.print("Conectando a Adafruit IO... ");
  int8_t ret;
  uint8_t intentos = 3;
  while ((ret = mqtt.connect()) != 0) {         
    mqtt.disconnect();
    delay(5000);
    if (--intentos == 0) ESP.restart();
  }
  Serial.println("Conectado.");
}