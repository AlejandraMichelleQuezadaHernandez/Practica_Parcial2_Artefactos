#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Credenciales de tu red Wi-Fi
#define WLAN_SSID       "CLARO_2.4GHz_99F78B"
#define WLAN_PASS       "claro1122"

// Credenciales de Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_Rtqi86HI5kOaHQHhYa9Vnl7Y7Ner"

#define PIN_TRIG 18
#define PIN_ECHO 19
int DIST_SELEC = 20;
bool alarmaArmada = false;
long unsigned tiempoAnterior = 0;
long unsigned intervalo = 2000;


// Variables globales para Wi-Fi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// --- CONFIGURACIÓN DE FEEDS ---
Adafruit_MQTT_Publish pub_ultra = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/alerta-distancia");
Adafruit_MQTT_Subscribe sub_estado = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/estado-alarma");

void sensorUltrasonico(){
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  int duracion = pulseIn(PIN_ECHO, HIGH);
  float distancia = (0.0343 * duracion) / 2;

  Serial.print("Distancia: ");
  Serial.println(distancia);
  delay(1000);
  if(distancia < 20 && alarmaArmada == true){
    if(pub_ultra.publish(distancia)){
      Serial.println("El feed se puedo publicar con exito.");
    }else{
      Serial.println("Fallo la publicación.");
    }
  }
}

void cambiarEstado(char *data, uint16_t longitud){
  if(strcmp(data, "ON") == 0){
    alarmaArmada = true;
  }else if(strcmp(data, "OFF") == 0){
    alarmaArmada = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");

  sub_estado.setCallback(cambiarEstado);
  mqtt.subscribe(&sub_estado);
}

void loop() {
  // Asegurar la conexión MQTT
  MQTT_connect();
  long unsigned tiempoActual = millis();
  // --- RETRASO Y ESCUCHA SIMULTÁNEA ---
  // En lugar de delay(5000), usamos processPackets(5000).
  // Esto hace una pausa de 5 segundos, pero procesa CUALQUIER mensaje que llegue en ese tiempo.
  mqtt.processPackets(10);
  if(tiempoActual - tiempoAnterior >= intervalo){
    tiempoAnterior = tiempoActual; 
    sensorUltrasonico();
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
