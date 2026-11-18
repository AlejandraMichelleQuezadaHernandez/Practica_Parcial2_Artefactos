#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Credenciales de tu red Wi-Fi para el simulador
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""

// Credenciales de Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"

#define PIN_IR 25

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

uint32_t contador = 0; 
int estadoAnterior = HIGH; // <--- Variable GLOBAL para recordar el botón

Adafruit_MQTT_Publish pub_infra = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/infrarrojo");

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // CRÍTICO: INPUT_PULLUP mantiene el pin en HIGH cuando no se toca
  pinMode(PIN_IR, INPUT_PULLUP);

  Serial.println(F("Conectando a Wi-Fi..."));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado.");
}

void loop() {
  MQTT_connect();
  mqtt.processPackets(10);

  int estadoActual = digitalRead(PIN_IR);

  // Solo cuenta si AHORA está presionado (LOW) pero ANTES no lo estaba (HIGH)
  if (estadoActual == LOW && estadoAnterior == HIGH) {
    contador = contador + 1;
    
    Serial.print("¡Objeto detectado! Total: ");
    Serial.println(contador);
    
    if(pub_infra.publish(contador)){
      Serial.println("Dato enviado a Adafruit.");
    }
    
    delay(100); // Pequeña pausa anti-rebote
  }

  // Guardamos el estado para el siguiente ciclo
  estadoAnterior = estadoActual;

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
    Serial.println(F("Reintentando conexión..."));
    mqtt.disconnect();
    delay(5000); 
  }
  Serial.println(F("¡MQTT Conectado!"));
}


