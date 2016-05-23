/* 
En este programa controlaremos via Bluetooth un motor paso a paso para accionar el slider que hemos desarrollado en nuestro proyecto.
Para ello usamos el driver A4988 y un arduino UNO.

Por:
Miguel Granero Ramos
Marta Salas Arroyo
Luis Lopez Berrocal
*/

#define fdcPin 7  //Pin del final de carrera

#define dirPin 9  //Pin de la direccion en el driver
#define stepPin 13 //Pin del driver para que el motor de un paso

int incomingByte; //Variable que usamos para guardar los datos enviados desde la App
int mode = 0; //Para cambiar entre modos de funcionamiento

int vel = 0; //Velocidad del slider
int posicion = 0; //Variable que guarda la posicion de la camara en el slider
int stepp = 0; //Variable que usaremos para saber en que direccion moverse en modo manual

//Variables modo automatico
int numDat = 0; //Indica el numero del dato que se ha enviado para almacenarlo en su lugar correspondiente
int runn = 0; //Indica si se ha iniciado el recorrido del slider
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
  pinMode(dirPin, OUTPUT); //Declaramos si los pines son INPUT o OUTPUT
  pinMode(stepPin, OUTPUT);
  pinMode(fdcPin, INPUT);
}

void manual() {
  //En este bloque indicamos al motor en que direccion girar en funcion de los datos recibidos
  if (incomingByte == 'H') { //H --> Derecha
    stepp = 1;
    Serial.println("Derecha");
  }
  if (incomingByte == 'E') { //E --> Izquierda
    stepp = -1;
    Serial.print("Izquierda");
  }
  if (incomingByte == 'L') { //L --> Parar
    stepp = 0;
    Serial.println("Parar");
  }
  //Este le indica la velocidad
  if (incomingByte <= 100) {
    vel = map(incomingByte, 0, 100, 200, 10); //Hacemos un map a la velocidad ya que esta se realiza mediante delays
  }
}

void automatico() { //Este bloque se encarga de la función principal del modo automatico
  //Si numDat es menor o igual a 3 (4 primeros datos) guardamos los valores en el array sliderpos
  if (numDat <= 3) {
    Serial.println(numDat);
    sliderpos[numDat] = 4 * incomingByte;
    Serial.println(sliderpos[numDat]);
  }
  //Si es mayor, lo guardamos en el array de la velocidad
  else {
    Serial.println(numDat);
    slidervel[numDat - 4] = incomingByte;
    Serial.println(slidervel[numDat - 4]);
  }
  //Al final de cada loop aumentamos la i en 1 para que el siguiente dato se guarde en el siguiente lugar del array.
  numDat++;
}

void play() {
  //En este bloque reseteamos las variables e iniciamos el proceso
  Serial.println("------Play------");
  numDat = 0;
  input = 0;
  runn = 1;
}

void mover(int pasos = 0) { //Este Bloque los usaremos para hacer que el motor se mueva en si
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
      vel = 10;
      //Cada vez que entra al modo manual se calibra automaticamente
      fdcState = digitalRead(fdcPin);
      while (fdcState == 0) {
        mover(1);
        fdcState = digitalRead(fdcPin);
      }
      posicion = 1000;

    }
    //Manual
    if (mode == 1) {
      manual(); //Llamamos al bloque manual
    }
    //Automatico
    if (mode == 2) {
      if (input == 1) {
        automatico();//Bloque automatico
      }
      if (numDat == 7) {
        play(); //Reseteamos la variables si ya se ha recibido el ultimo dato
      }
    }
    if (incomingByte == 'z' && input == 0) input = 1; //Antes de enviar los 6 datos se envia una z para que habilite la entrada de datos
  }


  //**************************

  if (mode == 1 && stepp != 0) mover(stepp);
  if (mode == 2 && runn == 1) {
    //En este codigo vamos a hacer que el slider vaya haciendo cada fase llendo una posicion con la velocidad dada.
    //Fase Inicial
    vel = 10;
    mover(sliderpos[0] - posicion);
    posicion = sliderpos[0];
    Serial.println(posicion);
    Serial.println(0);
    //Fase 1,2 y 3.
    for (int x=1; x < 4; x++) {
      vel = map((slidervel[x - 1]), 0, 100, 200, 10);
      mover(sliderpos[x] - posicion);
      posicion = sliderpos[x];
      Serial.println(posicion);
      Serial.println(x);
    }
    runn = 0; //Indicamos que ya se ha terminado el recorrido.
  }
}
