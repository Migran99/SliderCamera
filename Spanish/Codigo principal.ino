//El motor da 5.3 vueltas para recorrer todo el slider

//Vamos a usar la libreria stepper.h para controlar el motor
#include <Stepper.h>
#define STEPS 200 //El motor da 200 pasos por cada vuelta completa
#define fdcPin 7

Stepper stepper(STEPS, 8, 9, 10, 11); //Definimos los pines del motor

int incomingByte; //Variable que usamos para guardar los datos enviados desde la App
int mode = 0; //Para cambiar entre modos de funcionamiento

int vel = 0; //Velocidad del slider
int posicion = 0; //Variable que guarda la posicion de la camara en el slider
int stepp = 0; //Variable que usaremos para saber en que direccion moverse en modo manual


int i = 0; //Variable de apoyo que usaremos en algunas estructuras del codigo

//Variables modo automatico
int running = 0; //Indica si se ha iniciado el recorrido del slider
int input = 0; //Si esta habilitada la entrada de datos o no
int sliderpos[4] = {
  0, 0, 0, 0
}; //Array de las tres posiciones del modo automatico y la posicion inical
int slidervel[3] = {
  0, 0, 0
}; //Array de las tres velocidades del modo automatico


int fdcState = 0; //Estado del final de carrera (pulsado o no)

void setup() {
  Serial.begin(9600); //Iniciamos el Serial para imprimir mensajes de informacion
}

int manual() {
  //En este bloque indicamos al motor en que direccion girar en funcion de los datos recibidos
  if (incomingByte == 'H') { //H --> Derecha
    stepp = 1;
  }
  if (incomingByte == 'E') { //E --> Izquierda
    stepp = -1;
  }
  if (incomingByte == 'L') { //L --> Parar
    stepp = 0;
  }
  //Este le indica la velocidad
  if (incomingByte <= 100) {
    vel = incomingByte;
    stepper.setSpeed(vel);
  }
}

int automatico() { //Este bloque se encarga de la función principal del modo automatico
  //Si la i es menor o igual a 3 (4 primeros datos) guardamos los valores en el array sliderpos
  if (i <= 3) {
    Serial.println(i);
    sliderpos[i] = 4 * incomingByte;
    Serial.println(sliderpos[i]);
  }
  //Si es mayor, lo guardamos en el array de la velocidad
  else {
    Serial.println(i);
    slidervel[i - 4] = incomingByte;
    Serial.println(slidervel[i - 4]);
  }
  //Al final de cada loop aumentamos la i en 1 para que el siguiente dato se guarde en el siguiente lugar del array.
  i++;
}

int play() {
  //En este bloque reseteamos las variables e iniciamos el proceso
  Serial.println("------Play------");
  i = 0;
  input = 0;
  running = 1;
}

void loop() {
  //Leemos el serial si hay algun dato
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    //Selección de modo
    if (incomingByte == 'm') {
      mode = 1;
      Serial.println("Modo manual");
    }
    if (incomingByte == 'M') {
      mode = 2;
      Serial.println("Modo automatico");
      stepper.setSpeed(50);
      fdcState = digitalRead(fdcPin);
      while (fdcState == 0) { //Realizamos una calibracion cada vez de empieza el modo automatico
        stepper.step(1);
        fdcState = digitalRead(fdcPin);
      }
      posicion = 250;

    }
    //Manual
    if (mode == 1) {
      manual(); //Llamamos al bloque manual
    }
    //Automatico
    if (mode == 2) {
      if (input == 1) {
        automatico(); //Llamamos al bloque automatico
      }
      if (i == 7) {
        play(); //Reseteamos la variables si ya se ha recibido el ultimo dato

      }
    }
    if (incomingByte == 'z' && input == 0) input = 1; //Antes de enviar los 6 datos se envia una z para que habilite la entrada de datos
  }


  //**************************

  if (mode == 1 && stepp != 0) stepper.step(stepp);
  if (mode == 2 && running == 1) {
    switch (i) {
      //En este codigo vamos a hacer que el slider vaya haciendo cada fase llendo una posicion con la velocidad dada.
      case 0://Posicion inicial
        stepper.setSpeed(60);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 1://Fase 1
        stepper.setSpeed(slidervel[i - 1]);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 2://Fase 2
        stepper.setSpeed(slidervel[i - 1]);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 3://Fase 3
        stepper.setSpeed(slidervel[i - 1]);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
        i = 0;
        running = 0;
    }
  }
  //Serial.println(posicion);
}
