/*
I'm using random() to switch between saving-position between each startup, because EEPROM is only stable in each location for approx. 100 000 writes. Since we have
to save the position for every degree the servo turns (i.e. multiple times per second, depending on the delay/pause), this will ensure that the arduino/program stays
stable for as long as possible. 
The alternative is to only save th position every few degrees, but testing revealed that even a small difference between the written
position of the servo and the actual position will cause the servo to stall. 
*/

#include <Servo.h>
#include <EEPROM.h>

#define servoPin      3       //what pin the servo is connected to
#define defaultPos   90       //"parking" position of wiper arm (straight up)
#define lastStorage   0       //at what byte in the EEPROM last servo position before powerloss can be found
#define pause        25       //minimum 10ms!

#define minPos       70       //where the wiper touches the left A-frame
#define maxPos      117       //where the wiper touches the right A-frame

#define wiperStopped 5
#define stopSignal 6

Servo servo;


int pos;
int turn;
int s;                        //where position should be stored in case of power loss

void setup() {
  pinMode(wiperStopped, OUTPUT);
  pinMode(stopSignal, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(wiperStopped, LOW);
  digitalRead(stopSignal);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  s = random(1,255);
  
  Serial.print(EEPROM.read(lastStorage));
  Serial.print('\t');
  Serial.print(s);
  Serial.println();
  
  servo.attach(servoPin);
  pos = EEPROM.read(EEPROM.read(lastStorage));
  EEPROM.write(lastStorage, s);
  if (pos > maxPos || pos < minPos) {
    pos = defaultPos;
  }
  servo.write(pos);

  if (pos <= defaultPos) turn = 1;
  else if (pos > defaultPos) turn = -1;

  while (pos != defaultPos) {
    pos += turn;
    servo.write(pos);
    delay(pause);
  }
  Serial.print(pos);
  Serial.println();
  digitalWrite(13, HIGH);
}

void loop() {
  if (pos <= minPos) {
    turn = 1;
    delay(pause * 5);
  }
  else if (pos >= maxPos) {
    turn = -1;
    delay(pause * 5);
  }

  pos += turn;
  servo.write(pos);
  EEPROM.write(s, pos);
  
  Serial.print(pos);
  Serial.println();
  digitalRead(stopSignal);

  if(pos == defaultPos && stopSignal == true){
    digitalWrite(wiperStopped, HIGH);
    while(true){}
  }
  
  delay(pause);
}
