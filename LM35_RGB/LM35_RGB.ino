int PIN_LM = 32;
int PIN_ROJO = 25;
int PIN_VERDE = 26;
int PIN_AZUL = 27;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_LM, INPUT);
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);
}


void alarma(float res){
  digitalWrite(PIN_ROJO, LOW);
  digitalWrite(PIN_VERDE, LOW);
  digitalWrite(PIN_AZUL, LOW);

  if(res < 28){
    digitalWrite(PIN_ROJO, HIGH);
    delay(1000);
  }else if(res >= 28 && res <= 29){
    digitalWrite(PIN_VERDE, HIGH);
    delay(1000);
  }else{
    digitalWrite(PIN_AZUL, HIGH);
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  float numerador = analogReadMilliVolts(PIN_LM);
  float resultado = numerador /10.0;

  alarma(resultado);
  Serial.print("La temperatura es de: ");
  Serial.println(resultado);

  
  delay(2000);
}
