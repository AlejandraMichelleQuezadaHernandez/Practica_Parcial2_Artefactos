int PIN_TOUCH = 4;

const int pinRojo = 25;
const int pinVerde = 26;
const int pinAzul = 27;
const int UMBRAL_TOUCH = 600;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);
}

void aplicarColor(int r, int g, int b) {
  analogWrite(pinRojo, r);
  analogWrite(pinVerde, g);
  analogWrite(pinAzul, b);
}

void loop() {
  int lectura = touchRead(PIN_TOUCH);
  Serial.println(lectura);
  if (lectura < UMBRAL_TOUCH) {
    aplicarColor(0, 255, 0);
  } else {
    aplicarColor(255, 0, 0);
  }

  delay(100);
}
