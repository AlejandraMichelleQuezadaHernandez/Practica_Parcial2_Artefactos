int arreglo[] = {25,27,12};
int contador = 3;

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 3; i++) {
    pinMode(arreglo[i], OUTPUT);
    digitalWrite(arreglo[i], LOW);
  }
}

void secuenciaDespegue(int arreglo[], int contador) {
  while (contador != 0) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(arreglo[i], HIGH);
    };

    delay(2000);

    for (int i = 0; i < 3; i++) {
      digitalWrite(arreglo[i], LOW);
    };

    delay(2000);
    contador--;
  }
}

void lucesDeVuelo(int arreglo[]) {
  digitalWrite(arreglo[0], HIGH);
  digitalWrite(arreglo[2], HIGH);
  for(int i = 0; i < 3; i++){
    digitalWrite(arreglo[1], HIGH);
    delay(1000);
    digitalWrite(arreglo[1], LOW);
    delay(1000);
  }
  
}

void alertaBateriaBaja(int arreglo[]) {
  digitalWrite(arreglo[0], LOW);
  digitalWrite(arreglo[1], LOW);
  for(int i = 0; i < 3; i++){
    digitalWrite(arreglo[2], HIGH);
    delay(500);
    digitalWrite(arreglo[2], LOW);
    delay(500);
  }
}

void loop(){
  secuenciaDespegue(arreglo, contador);
  delay(2000);
  lucesDeVuelo(arreglo);
  delay(2000);
  alertaBateriaBaja(arreglo);
  delay(2000);
}
