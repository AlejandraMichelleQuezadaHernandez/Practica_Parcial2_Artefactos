int PIN_LDR = 0;
int PIN_ROJO = 25;
int PIN_VERDE = 26;
int PIN_AZUL = 27;
float UMBRAL = 2.5;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);
}

void asignacionColores(int r, int g, int b){
  analogWrite(PIN_ROJO, r);
  analogWrite(PIN_VERDE, g);
  analogWrite(PIN_AZUL, b);

}

void loop() {
  int lectura = analogRead(PIN_LDR);

  float resultado = 3.3 * (lectura / 4095.0);

  Serial.print("Valor esperado: ");
  Serial.println(resultado);

  if(resultado < UMBRAL){
    asignacionColores(231,20,255);
  }else{
    asignacionColores(170,100,54);
  }
  delay(2000);
}
