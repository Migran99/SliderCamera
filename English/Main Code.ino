/*
  This is the code used for the communication between the App and Arduino
  In this case we are using LEDs instead of the stepper motor in order to make it more visual

  Miguel Granero Ramos
  Luis Lopez Berrocal
  Marta Salas Arroyo
  IES Vicente Aleixandre 2016
*/

#define ledRPin 6
#define ledGPin 5

int incomingByte;             //Stores the data recieved in the Serial
int mode = 0;                 //This variable is used to change between modes
int sliderpos[4] = {0 ,0, 0, 0}; //Positions array
int slidervel[3] = {0, 0, 0}; //Speeds array
int i = 0;                    //We'll use this variable as a guide for the data in the arrays
int input = 0;                //INPUT will be use to let arduino know when the data is going to be sent
//It is need in order to avoid noise in the communication

void setup() {
  //Initiate the serial at 9600 bauds
  Serial.begin(9600);
  pinMode(ledRPin, OUTPUT);
  pinMode(ledGPin, OUTPUT);
}

void manual() {
  //This block is the manual control mode
  if (incomingByte == 'H') { //If we receive the letter H, motor will go left
    digitalWrite(ledRPin, HIGH);
    Serial.println("Left");
  }
  if (incomingByte == 'E') {//If we receive the letter E, motor will go right
    digitalWrite(ledGPin, HIGH);
    Serial.println("Right");
  }
  if (incomingByte == 'L') {//If we receive the letter L, motor will stop
    digitalWrite(ledRPin, LOW);
    digitalWrite(ledGPin, LOW);
    Serial.println("Stop");
  }
}

int automatico() { //This block is the automatic control mode
  //If i is less or equal to 3 (4 first values) we save the values in the sliderpos array
  if (i <= 3) {
    Serial.print("i= ");
    Serial.println(i);
    sliderpos[i] = incomingByte;
    Serial.println(sliderpos[i]);
  }
  //If it is greater than 3, we save it in slidervel array
  else {
    Serial.print("i= ");
    Serial.println(i);
    slidervel[i - 4] = incomingByte;
    Serial.println(slidervel[i - 4]);
  }
  //We add 1 to i at the end of each loop
  i++;
  return i;
}

int reset() {
  //Reset of all the variables need in the automatic mode in order to be able to receive data again
  Serial.println("------Reset------");
  i = 0;
  input = 0;
  return i;
  return input;
}

void loop() {
  if (Serial.available() > 0) {
    //If there is any data stored in the Serial we read it and print and message
    incomingByte = Serial.read();
    Serial.println("Data received......");

    //Mode selection(m = manual ; M=automatic)
    if (incomingByte == 'm') {
      mode = 1;
      Serial.println("Manual mode");
    }
    if (incomingByte == 'M') {
      mode = 2;
      Serial.println("Automatic mode");
    }

    //Manual mode
    if (mode == 1) {
      manual(); //It calls the block manual();
    }

    //Automatic mode
    if (mode == 2) {
      //If data input is available
      if (input == 1) {
        automatico(); //We call the block automatic();
        if (i == 7) {
          reset(); //Reset all the data calling the block reset();
        }
      }
      if (incomingByte == 'z' && input == 0) input = 1; //Before we send all the values we send a 'z' so we enable the input

    }
  }
}
