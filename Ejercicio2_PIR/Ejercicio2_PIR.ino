#include <WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// --- CREDENCIALES DE RED Y ADAFRUIT ---
#define WLAN_SSID       "Wokwi-GUEST"
#define WLAN_PASS       ""
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "michiis"
#define AIO_KEY         "aio_tjyE87X5Kzj7CWVJODaNcXwLyrAW"

// --- VARIABLES DEL SENSOR Y RELOJ ---
#define PIN_OUT 22
int contadorPersonas = 0;
int umbral = 4;
unsigned long tiempoTranscurrido = 30000; // 15 segundos
unsigned long tiempoInicio = 0;
int estadoAnterior = LOW; // El PIR inicia en LOW (sin movimiento)

// --- OBJETOS DE CONEXIÓN ---
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish pub_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/control-personas");

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(PIN_OUT, INPUT); // Wokwi PIR usa INPUT normal

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

  // 2. Tomar el tiempo actual y leer el sensor (Los ojos de Robotín)
  unsigned long tiempoActual = millis();
  int estadoActual = digitalRead(PIN_OUT);

  // ==========================================
  // TAREA 1: VIGILAR LA PUERTA (Todo el tiempo)
  // ==========================================
  // Si detecta un cambio de LOW a HIGH (alguien acaba de pasar)
  if (estadoActual == HIGH && estadoAnterior == LOW ) {
    contadorPersonas++; 
    Serial.print("¡Movimiento detectado! Llevamos: ");
    Serial.println(contadorPersonas);
  }

  // ==========================================
  // TAREA 2: REVISAR EL RELOJ (Cada 15 segundos)
  // ==========================================
  // Si la resta del tiempo actual menos el inicio ya llegó a 15000 ms
  if (tiempoActual - tiempoInicio >= tiempoTranscurrido) {
    
    Serial.println("--- CERRANDO VENTANA DE 15 SEGUNDOS ---");
    
    // A. Evaluamos el umbral (Factor de dominio)
    if (contadorPersonas > umbral) {
      Serial.println("Estado: Tráfico ALTO");
    } else {
      Serial.println("Estado: Tráfico BAJO");
    }

    // B. Publicamos a Adafruit SOLO una vez cada 15 seg
    if(pub_feed.publish((uint32_t)contadorPersonas)){
      Serial.println("Enviado a Adafruit con éxito.");
    } else {
      Serial.println("Error al enviar a Adafruit.");
    }
    Serial.println("---------------------------------------");

    // C. Reiniciamos la libreta y el reloj para el siguiente turno
    tiempoInicio = tiempoActual; 
    contadorPersonas = 0;
  }

  // ==========================================
  // TAREA 3: GUARDAR MEMORIA Y DESCANSAR
  // ==========================================
  // Guardamos cómo estaba el sensor para compararlo en el siguiente milisegundo
  estadoAnterior = estadoActual;
  
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