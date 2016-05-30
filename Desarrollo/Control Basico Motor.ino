/*
  Este es el codigo basico para controlar un motor paso a paso mediante un driver A4988

  Miguel Granero Ramos
  Marta Salas Arroyo
  Luis Lopez Berrocal
*/

#define dirPin 9
#define stepPin 13
int pasos = 0;
int vel = 10;

void setup() {
  Serial.begin(9600); //Iniciamos el Serial para imprimir mensajes de informacion
  pinMode(dirPin, OUTPUT); //Declaramos si los pines son INPUT o OUTPUT
  pinMode(stepPin, OUTPUT);
}

void mover(int pasos = 0) { //Este Bloque los usaremos para hacer que el motor se mueva
  //Cambiamos la direccion dependiendo de si el numero introducido es positivo o negativo
  if (pasos >= 0) digitalWrite(dirPin, HIGH);
  else {
    digitalWrite(dirPin, LOW);
    pasos = pasos * -1;
  }
  //En este for hacemos que el motor se accione el numero de veces que le hayamos introducido
  for (int x = 0; x < pasos; x++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delay(vel); //Este delay funciona regulando la velocidad del motor
  }
}

void loop() {
  if (Serial.available() > 0) { //Recibimos el valor que le hemos introducido por el Serial
    pasos = Serial.read();
    Serial.print("-----Data Recibido-----");
    Serial.println(pasos);
    mover(pasos); //Llamamos al bloque mover introduciendole el numero de pasos que queremos que se mueva
  }
}
