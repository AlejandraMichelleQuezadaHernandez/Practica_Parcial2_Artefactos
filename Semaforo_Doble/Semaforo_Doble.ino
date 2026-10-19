// C++ code
//

int semaforo1[]= {25,26,27};
int semaforo2[]= {14,12,13};
int verde = 0;
int amarillo = 1;
int rojo = 2;
void setup()
{
  for(int i = 0; i < 3; i++){
    pinMode(semaforo1[i], OUTPUT);
    pinMode(semaforo2[i], OUTPUT);
    digitalWrite(semaforo1[i], LOW);
    digitalWrite(semaforo2[i],LOW);
  };
}
  void encenderSemaforo(int semaforo[], int color){
    for(int i = 0; i < 3; i++){
      digitalWrite(semaforo[i], LOW);
    }
    
    digitalWrite(semaforo[color], HIGH);
  };

void loop(){
  
  encenderSemaforo(semaforo1, verde);
  encenderSemaforo(semaforo2, rojo);
  
  delay(2000);
  
  encenderSemaforo(semaforo1, amarillo);
  encenderSemaforo(semaforo2, rojo);
  
  delay(2000);
  
  encenderSemaforo(semaforo1, rojo);
  encenderSemaforo(semaforo2, verde);
  
  delay(2000);
  
  encenderSemaforo(semaforo1, rojo);
  encenderSemaforo(semaforo2, amarillo);
  
  delay(2000);
  
}