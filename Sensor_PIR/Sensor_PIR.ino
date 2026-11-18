#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Credenciales de tu red Wi-Fi
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""

// Credenciales de Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"

#define PIN_PIR 4
// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_PIR = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/control-pir");
Adafruit_MQTT_Subscribe sub_feed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/interruptor");
uint32_t contador = 0;
int estadoAnterior = LOW;

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(PIN_PIR, INPUT);
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

  // --- BLOQUE DE PUBLICACIÓN ---
  int estado = digitalRead(PIN_PIR);

  if(estado == HIGH && estadoAnterior == LOW){
    contador++;
    if(pub_PIR.publish(contador)){
      Serial.println("Valor publicado con exito");
    }
    delay(2000);
  }
  estadoAnterior = estado;

  // --- RETRASO Y ESCUCHA SIMULTÁNEA ---
  // En lugar de delay(5000), usamos processPackets(5000).
  // Esto hace una pausa de 5 segundos, pero procesa CUALQUIER mensaje que llegue en ese tiempo.
  mqtt.processPackets(10);

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