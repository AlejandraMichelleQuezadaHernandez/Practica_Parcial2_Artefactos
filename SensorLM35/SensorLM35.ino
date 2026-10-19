int SEN_LM = 4;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SEN_LM, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  int lectura = analogRead(SEN_LM);

  float conversion = (3300.0 * lectura)/4095.0;
  float resultado = conversion/10.0;

  Serial.print("La temperatura es: ");
  Serial.println(resultado);
  delay(2000);
}
