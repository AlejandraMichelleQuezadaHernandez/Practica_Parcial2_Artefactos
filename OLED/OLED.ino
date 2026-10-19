#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

int sensor;
float temp; 

Adafruit_SH1106G display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

void setup() {

  Serial.begin(9600);

  
  Wire.begin();

  delay(250);
  

}

void loop() {

  sensor = analogRead(A0);

  temp = (sensor *5.0/1023.0) * 100;

  
  if (!display.begin(i2c_Address, true)) {
    Serial.println("ERROR: OLED no encontrada");

    while (1);
  }

  Serial.println("OLED funcionando correctamente");

  
  display.clearDisplay();

  
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(2);
  display.setCursor(15, 10);
  display.println("Temperatura");
  display.println(temp);
  
  display.display();
  Serial.println("Temperatura");
  Serial.println(temp);

  delay(2000);
}