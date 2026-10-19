#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Credenciales de tu red Wi-Fi
#define WLAN_SSID "CLARO_2.4GHz_99F78B"
#define WLAN_PASS "claro1122"

// Credenciales de Adafruit IO
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "michiis"
#define AIO_KEY "aio_GTfj49dw6cPHmt2tekUURbndFaao"

//Sensores conectados a la placa
#define LED_ROJO 25
#define LED_VERDE 26
#define LED_AZUL 27

// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperatura");
Adafruit_MQTT_Subscribe SubLedRGB = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/controlrgb");

//Creación de función de estado
void cambioColor(char *data, uint16_t len) {
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AZUL, LOW);

  if (strcmp(data, "ROJO") == 0) {
    digitalWrite(LED_ROJO, HIGH);
  } else if (strcmp(data, "VERDE") == 0) {
    digitalWrite(LED_VERDE, HIGH);
  } else if (strcmp(data, "AZUL") == 0) {
    digitalWrite(LED_AZUL, HIGH);
  } 
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
  SubLedRGB.setCallback(cambioColor);
  mqtt.subscribe(&SubLedRGB);
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();

  mqtt.processPackets(10000);
  // --- BLOQUE DE PUBLICACIÓN ---
  float valorSensor = random(20, 35);  // Simulación de tu sensor

  Serial.print(F("<-- Publicando dato en Adafruit... "));
  Serial.print(valorSensor);

  if (!pub_feed.publish(valorSensor)) {
    Serial.println(F(" ¡Falló la publicación!"));
  } else {
    Serial.println(F(" ¡Publicado con éxito!"));
  }

  // --- RETRASO Y ESCUCHA SIMULTÁNEA ---
  // En lugar de delay(5000), usamos processPackets(5000).
  // Esto hace una pausa de 5 segundos, pero procesa CUALQUIER mensaje que llegue en ese tiempo.
  mqtt.processPackets(5000);

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
    delay(5000);  // Este delay solo ocurre si se cae el servidor
  }
  Serial.println(F("¡MQTT Conectado!"));
}
