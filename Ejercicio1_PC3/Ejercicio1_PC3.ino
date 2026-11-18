#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// Credenciales
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish pub_alarma = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/manejo-alarma");

int PIN_IR = 25;
bool estado = false;            
unsigned long tiempoInicio = 0; 
unsigned long tiempoFinal = 0;
unsigned long umbral = 0;
int estadoAnterior = HIGH;      
bool alarmaEnviada = false;     

void setup() {
  Serial.begin(115200);
  pinMode(PIN_IR, INPUT_PULLUP);

  Serial.println("Conectando a Wi-Fi...");
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

  // FASE 1: CALIBRACIÓN
  if (estado == false) {
    if (estadoActual == LOW && estadoAnterior == HIGH) {
      tiempoInicio = millis();
      Serial.println("Calibrando: Caja entrando...");
    }

    if (estadoActual == HIGH && estadoAnterior == LOW) {
      tiempoFinal = millis();
      umbral = (tiempoFinal - tiempoInicio) + 2000; 
      Serial.print("Calibración exitosa. Umbral: ");
      Serial.println(umbral);
      estado = true; 
    }
  }

  // FASE 2: MONITOREO DE ATASCOS
  if (estado == true) {
    
    // Llegó caja
    if (estadoActual == LOW && estadoAnterior == HIGH) {
      tiempoInicio = millis(); 
      alarmaEnviada = false;   
    }

    // Evaluación del umbral (La trampa)
    if (estadoActual == LOW && (millis() - tiempoInicio > umbral) && alarmaEnviada == false) {
      Serial.println("¡ALERTA DE ATASCO!");
      pub_alarma.publish(1);   
      alarmaEnviada = true;    
    }

    // Salió caja
    if (estadoActual == HIGH && estadoAnterior == LOW) {
      Serial.println("Banda despejada, flujo normal.");
    }
  }

  estadoAnterior = estadoActual;
  
  // Este delay es CLAVE para que la placa no crashee en Wokwi
  delay(50); 
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Conectando a MQTT... ");
  while ((ret = mqtt.connect()) != 0) { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando en 5 segundos...");
    mqtt.disconnect();
    delay(5000); 
  }
  Serial.println("¡MQTT Conectado!");
}