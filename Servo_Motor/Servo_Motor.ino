#include <ESP32Servo.h>

int pos = 0; 

Servo servo;
void setup() {
  // put your setup code here, to run once:
  servo.attach(26, 500, 2500);
}


void loop() {
  for (pos = 0; pos <= 180; pos += 1) { 
    servo.write(pos);   
    delay(15);        
  }
  
  for (pos = 180; pos >= 0; pos -= 1) { 
    servo.write(pos);   
    delay(15);        
  }
}
