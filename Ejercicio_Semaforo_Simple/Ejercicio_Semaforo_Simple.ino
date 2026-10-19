int g = 3;
int y = 5;
int r = 9;
int v = 10;
int a = 4;
int ro = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(g, OUTPUT);
  pinMode(y, OUTPUT);
  pinMode(r, OUTPUT);
  pinMode(v, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(ro, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(g, HIGH);
  digitalWrite(y, LOW);
  digitalWrite(r, LOW);

  digitalWrite(v, LOW);
  digitalWrite(a, LOW);
  digitalWrite(ro, HIGH);

  delay(2000);


  digitalWrite(g, LOW);
  digitalWrite(y, HIGH);
  digitalWrite(r, LOW);

  digitalWrite(v, LOW);
  digitalWrite(a, LOW);
  digitalWrite(ro, HIGH);

  delay(2000);

  digitalWrite(g, LOW);
  digitalWrite(y, LOW);
  digitalWrite(r, HIGH);

  digitalWrite(v, HIGH);
  digitalWrite(a, LOW);
  digitalWrite(ro, LOW);

  delay(2000);

  digitalWrite(g, LOW);
  digitalWrite(y, LOW);
  digitalWrite(r, HIGH);

  digitalWrite(v, LOW);
  digitalWrite(a, HIGH);
  digitalWrite(ro, LOW);
  delay(2000);



}
