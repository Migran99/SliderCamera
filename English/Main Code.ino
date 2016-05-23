/* 
This is the code used for the communication between the App and Arduino.
We'll use an A4988 driver and an Arduino UNO

Miguel Granero Ramos
Marta Salas Arroyo
Luis Lopez Berrocal
*/

#define fdcPin 7  //Switch Pin

#define dirPin 9  //Direction Pin of the driver
#define stepPin 13 //Pin used to make the motor to move

int incomingByte; //Variable used to receive the data from the APP
int mode = 0; //Change between modes

int vel = 0; //Speed of the slider
int posicion = 0; //It stores the position of the slider
int stepp = 0; //It's used to know the direction in which move within the manual mode

//Variables modo automatico
int numDat = 0; //Indicates position of the data received
int runn = 0; ////Indicates if the slider is running or not
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
  pinMode(dirPin, OUTPUT); //Declare if the pins ar INPUT or OUTPUT
  pinMode(stepPin, OUTPUT);
  pinMode(fdcPin, INPUT);
}

void manual() {
  //We indicate the motors in which direction turn depending on the data received
  if (incomingByte == 'H') { //H --> RIGHT
    stepp = 1;
    Serial.println("Right");
  }
  if (incomingByte == 'E') { //E --> LEFT
    stepp = -1;
    Serial.print("Left");
  }
  if (incomingByte == 'L') { //L --> STOP
    stepp = 0;
    Serial.println("Stop");
  }
   //This sets the speed of the motor
  if (incomingByte <= 100) {
    vel = map(incomingByte, 0, 100, 200, 10); //We make a map to speed since it is made with delay
  }
}

void automatico() { //This is in charge of the main part of the automatic mode
  //We store the position data
  if (numDat <= 3) {
    Serial.println(numDat);
    sliderpos[numDat] = 4 * incomingByte;
    Serial.println(sliderpos[numDat]);
  }
  //We store the speed data
  else {
    Serial.println(numDat);
    slidervel[numDat - 4] = incomingByte;
    Serial.println(slidervel[numDat - 4]);
  }
  //Add 1 to numDat to change the position in the Arrays
  numDat++;
}

void play() {
 //This resets the variables for the autmatic mode and initiate the process
  Serial.println("------Play------");
  numDat = 0;
  input = 0;
  runn = 1;
}

void mover(int pasos = 0) { //We'll use this to make the motor to move
  //Changes the direction depending on the sing of the given number (pasos)
  if (pasos >= 0) digitalWrite(dirPin, HIGH); 
  else {
    digitalWrite(dirPin, LOW);
    pasos = pasos * -1;
  }
  //The motor moves (pasos) times
  for (int x = 0; x < pasos; x++) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delay(vel); //This delay set the speed of the motor
  }
}

void loop() {
  //We read the serial if there is somethingo
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
      vel = 10;
      //This calibrates the position of the slider automatically
      fdcState = digitalRead(fdcPin);
      while (fdcState == 0) {
        mover(1);
        fdcState = digitalRead(fdcPin);
      }
      posicion = 1000;

    }
    //Manual
    if (mode == 1) {
      manual(); //Call the manual block
    }
    //Automatico
    if (mode == 2) {
      if (input == 1) {
        automatico();//Call the automatic mode
      }
      if (numDat == 7) {
        play(); //Call the block play to reset variables
      }
    }
    if (incomingByte == 'z' && input == 0) input = 1; //We send an 'z' before sending all the data from the APP
  }


  //**************************

  if (mode == 1 && stepp != 0) mover(stepp); //If manual mode, the motor moves in one direction or stays still
  if (mode == 2 && runn == 1) {
    //In this code we make the slider to follow all the phases of the automatic mode. Going from position to another at a given speed.
    //Starting position
    vel = 10;
    mover(sliderpos[0] - posicion);
    posicion = sliderpos[0];
    Serial.println(posicion);
    Serial.println(0);
    //Phase 1,2 and 3.
    for (int x=1; x < 4; x++) {
      vel = map((slidervel[x - 1]), 0, 100, 200, 10);
      mover(sliderpos[x] - posicion);
      posicion = sliderpos[x];
      Serial.println(posicion);
      Serial.println(x);
    }
    runn = 0; //Indicates that the the automatic mode has finished
  }
}
