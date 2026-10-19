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
bool activarTolva = false; 

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_estado = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/estado-sistema");
Adafruit_MQTT_Subscribe sub_dispensar = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/comando-dispensar");



void tolvaConfirmacion(char *data, uint16_t longitud){
  if(strcmp(data, "1")== 0){
    servo.write(90);
    delay(2000);
    servo.write(0);
    activarTolva = true;
  }
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

  sub_dispensar.setCallback(tolvaConfirmacion);
  mqtt.subscribe(&sub_dispensar);
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();

  // --- RETRASO Y ESCUCHA SIMULTÁNEA ---
  // En lugar de delay(5000), usamos processPackets(5000).
  // Esto hace una pausa de 5 segundos, pero procesa CUALQUIER mensaje que llegue en ese tiempo.
  mqtt.processPackets(5000);

  if(activarTolva == true){
    pub_estado.publish("Dispensador OK");
    activarTolva = false;
  }else{
    pub_estado.publish("No esta bien");
  }

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
