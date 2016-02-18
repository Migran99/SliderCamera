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
int sliderpos[3] = {0, 0, 0}; //Positions array
int slidervel[3] = {0, 0, 0}; //Speeds array
int i = 0;                    //We'll use this variable as a guide for the data in the arrays
int input = 0;                //INPUT will be use to let arduino know when
                              //Esta variable es necesaria para evitar ruidos y fallos en la comunicacion

void setup() {
  //Iniciamos el serial a una velocidad de 9600 y declaramos los pines como OUTPUT
  Serial.begin(9600);
  pinMode(ledRPin, OUTPUT);
  pinMode(ledGPin, OUTPUT);
}

void manual() {
  //Este bloque se encarga del modo manual
  if (incomingByte == 'H') { //Si recibimos la letra H, el rail ira hacia la izquierda
    digitalWrite(ledRPin, HIGH);
    Serial.println("Izquierda");
  }
  if (incomingByte == 'E') {//Si recibimos la letra E, el rail ira hacia la derecha
    digitalWrite(ledGPin, HIGH);
    Serial.println("Derecha");
  }
  if (incomingByte == 'L') {//Si recibimos la letra L, el rail parara
    digitalWrite(ledRPin, LOW);
    digitalWrite(ledGPin, LOW);
    Serial.println("Stop");
  }
}

int automatico() { //Este bloque se encarga de la función principal del modo automatico
  //Si la i es menor o igual a 2 (3 primeros datos) guardamos los valores en el array sliderpos
  if (i <= 2) {
    Serial.println(i);
    sliderpos[i] = incomingByte;
    Serial.println(sliderpos[i]);
  }
  //Si es mayor, lo guardamos en el array de la velocidad
  else {
    Serial.println(i);
    slidervel[i - 3] = incomingByte;
    Serial.println(slidervel[i - 3]);
  }
  //Al final de cada loop aumentamos la i en 1 para que el siguiente dato se guarde en el siguiente lugar del array.
  i++;
  return i;
}

 int reset() {
  //En este bloque reseteamos las variables del automatico para que se puedan volver a enviar datos
  Serial.println("------Reset------");
  i = 0;
  input = 0;
  return i;
  return input;
}

void loop() {
  if (Serial.available() > 0) {
    //Si hay algo en el serial lo leemos y avisamos imprimiendo un mensaje
    incomingByte = Serial.read();
    Serial.println("Data recibido......");

    //Selección de modo (m = manual ; M=automatico)
    if (incomingByte == 'm') {
      mode = 1;
      Serial.println("Modo manual");
    }
    if (incomingByte == 'M') {
      mode = 2;
      Serial.println("Modo automatico");
    }

    //Modo manual
    if (mode == 1) {
      manual(); //Llamamos al bloque manual(); ya comentado antes
    }

    //Modo Automático
    if (mode == 2) {
      //Si el modo es automatico y la entrada de datos esta habilitada
      if (input == 1) {
        automatico(); //Llamamos al bloque automatico();
        if (i == 6) {
          reset(); //Reseteamos la variables si ya se ha recibido el ultimo dato
        }
      }
      if (incomingByte == 'z' && input == 0) input = 1; //Antes de enviar los 6 datos se envia una z para que habilite la entrada de datos
    }
    
  }
}
