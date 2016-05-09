/*
  This is the code used for the communication between the App and Arduino

  Miguel Granero Ramos
  Luis Lopez Berrocal
  Marta Salas Arroyo
  IES Vicente Aleixandre 2016
*/
//El motor da 5.3 vueltas para recorrer todo el slider

//We're using the library Stepper.h to control the motor
#include <Stepper.h>
#define STEPS 200 //The motor makes 200 steps per revolution
#define fdcPin 7

Stepper stepper(STEPS, 8, 9, 10, 11); //We define the pins of the motor

int incomingByte; //Variable used to receive the data from the APP
int mode = 0; //Change between modes

int vel = 0; //Speed of the slider
int posicion = 0; //It stores the position of the slider
int stepp = 0; //It's used to know the direction in which move within the manual mode


int i = 0; //This is a support variable that is used in many parts of the code

//Automatic mode variables
int running = 0; //Indicates if the slider is running or not
int input = 0; //If the data input is open
int sliderpos[4] = {
  0, 0, 0, 0
}; //Array of the positions for the automatic mode
int slidervel[3] = {
  0, 0, 0
}; //Array of the speeds for the automatic mode


int fdcState = 0; //State of the switch

void setup() {
  Serial.begin(9600); //Iniciate the serial
}

int manual() {
  //We indicate the motors in which direction turn depending on the data received
  if (incomingByte == 'H') { //H --> RIGHT
    stepp = 1;
  }
  if (incomingByte == 'E') { //E --> LEFT
    stepp = -1;
  }
  if (incomingByte == 'L') { //L --> STOP
    stepp = 0;
  }
  //This sets the speed of the motor
  if (incomingByte <= 100) {
    vel = incomingByte;
    stepper.setSpeed(vel);
  }
}

int automatico() { //This is in charge of the main part of the automatic mode
  //We store the position data
  if (i <= 3) {
    Serial.println(i);
    sliderpos[i] = 4 * incomingByte;
    Serial.println(sliderpos[i]);
  }
  //We store the speed data
  else {
    Serial.println(i);
    slidervel[i - 4] = incomingByte;
    Serial.println(slidervel[i - 4]);
  }
  i++;
}

int play() {
  //This resets the variables for the autmatic mode and initiate the process
  Serial.println("------Play------");
  i = 0;
  input = 0;
  running = 1;
}

void loop() {
  //We read the serial if there is something
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    //Mode selection
    if (incomingByte == 'm') {
      mode = 1;
      Serial.println("Modo manual");
    }
    if (incomingByte == 'M') {
      mode = 2;
      Serial.println("Modo automatico");
      stepper.setSpeed(50);
      digitalRead(fdcPin);
      while (fdcState == 0) { //This calibrates the position of the slider
        stepper.step(1);
        fdcState = digitalRead(fdcPin);
      }
      posicion = 250;

    }
    //Manual
    if (mode == 1) {
      manual(); //Call the manual block
    }
    //Automatico
    if (mode == 2) {
      if (input == 1) {
        automatico(); //Call the automatic mode
      }
      if (i == 7) {
        play(); //Call the block play to reset variables

      }
    }
    if (incomingByte == 'z' && input == 0) input = 1; //We send an 'z' before sending all the data from the APP
  }


  //**************************

  if (mode == 1 && stepp != 0) stepper.step(stepp);
  if (mode == 2 && running == 1) {
    switch (i) {
      //In this code we make the slider to follow all the phases of the automatic mode. Going from position to another at a given speed.
      case 0://Starting position
        stepper.setSpeed(60);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 1://Phase 1
        stepper.setSpeed(slidervel[i - 1]);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 2://Phase 2
        stepper.setSpeed(slidervel[i - 1]);
        stepper.step(sliderpos[i] - posicion);
        posicion = sliderpos[i];
        i++;
        Serial.println(posicion);
        Serial.println(i);
      case 3://Phase 3
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
