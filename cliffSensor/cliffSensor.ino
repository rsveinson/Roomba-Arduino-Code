#include <SoftwareSerial.h>

#define rxPin 10
#define txPin 11

SoftwareSerial softSerial = SoftwareSerial(rxPin, txPin);

int inByte = 0;

void setup() {
  delay(2000);    // wait 2 secobnds to allow robot to initialize
  
  // define the pin modes
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  //softSerial.write((byte)10);
  softSerial.begin(19200);
  Serial.begin(9600);
  
  softSerial.write(128);  // starts OI
  softSerial.write(131);
  //softSerial.write((byte)10);
 //softSerial.begin(57600);
  

  // put your setup code here, to run once:
  
  Serial.println("starting sensor " + inByte);

}

void loop() {
  // put your main code here, to run repeatedly:
  softSerial.write(142); // send a senosr packet the next line indicate the packet id
  softSerial.write(9);  // as an examle of reading sensor data read the left cliff sensor
  delay(250);
  if(softSerial.available() > 0){
    inByte = softSerial.read();
    Serial.println(inByte);
  }
  
  //Serial.println(inByte);

}
