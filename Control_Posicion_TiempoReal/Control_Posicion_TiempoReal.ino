#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP32Servo.h>

// Credenciales de tu red Wi-Fi
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""

// Credenciales de Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_Rtqi86HI5kOaHQHhYa9Vnl7Y7Ner"

#define PIN_SERVO 26
// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Servo servo; 

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperatura");
Adafruit_MQTT_Subscribe sub_servo = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/angulo-servo");



void anguloMotor(double anguloSeleccionado){
  Serial.print("Valor leído del servo motor: ");
  Serial.print(anguloSeleccionado);
  Serial.println(" grados.");
  servo.write((int)anguloSeleccionado);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  servo.attach(PIN_SERVO);

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");

  sub_servo.setCallback(anguloMotor);
  mqtt.subscribe(&sub_servo);
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();

  // --- BLOQUE DE PUBLICACIÓN ---
  float valorSensor = random(20, 35); // Simulación de tu sensor

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
    delay(5000); // Este delay solo ocurre si se cae el servidor
  }
  Serial.println(F("¡MQTT Conectado!"));
}
