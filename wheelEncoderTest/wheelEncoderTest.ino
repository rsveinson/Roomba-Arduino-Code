#include<SoftwareSerial.h>

const float PIE = 3.14159;

int inByte = 0;
int n = 0;              // simple counter for loop()
int rightEncoderDistance = 0;
int leftEncoderDistance = 0;
float turnAngle = 0.0;

float angleDegrees = 0;

byte encoderData[4];

// global scope
int rxPin = 9;
int txPin = 11;
int ddPin = 5;
int buttonPin = 12;           // hold operation until button is pressed

SoftwareSerial mySerial(rxPin, txPin);  // new port with 10rx and 11tx
                                  // this means pin 4 on roomba connects to pin 10 on arduino
                                  // pin 3 on roomba connects to pin 11 on arduino

/*****************************************************************
 * start the serial port to print to serial monitor
 * print opening message to serial monitor
 * 
 * start mySerial at 19200 baud
 * 
 * put roomba into passive mode opcode 128
 * put roomba into safe mode opcode 130 or 131, they seem to do the same thing
 */
void setup(){
  Serial.begin(9600);       // start serial port for monitor
  //Serial.println("hello dave");

  mySerial.begin(19200);       // start software serial for communicating with roomba
                                // use software serial because roomba's tx doesn't always
                                // carry enough voltage to meet arduino's requirements
  delay(10);              // set a short delay to allow the command to propegate
  
  pinMode (ddPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // *** wake up roomba
  //Serial.println("PUSH BUTTON TO WAKE UP ROOMBA");
  //waitForButton();
  //delay(2000);
  wakeUp(); // 2 second delay built into the wakeUp() function
  
  // ** set roomba mode
  mySerial.write(128);        // put roomba into passive mode
  delay(20);                  // clean light should come on

  mySerial.write(131);        // put roomba into safe mode
  delay(20);                  // clean light should go out
                              // roomba is now ready to receive commands through the din port
  Serial.println(mySerial.read());
  
  readyDance();               // jiggle to signal ready to go
  playBeep();                 // beep once to indicate waiting for button press
  waitForButton();            // wait for the button to be pushed
  
  //Serial.println("roomba initialized into safe mode, ready to go");

  //goForward();
  //driveWheels(150, 250);
  //delay(2000);
  //stopDrive();
  //Serial.println("after forward and stop");



  //driveWheels(250,150);
  //delay(2000);   // for 2 seconds
  //stopDrive();
  //delay(500);

  //driveWheels(150, 150);
  //delay(2000);

  //driveWheels(-250, -250);
  //delay(2000);
  //Serial.println("after reverse");
  
  //goForward();
  //delay(2000);

  //Serial.println("shutting down, (off mode, opcode 173).");
  //mySerial.write(173);        // change to off mode.
   float tempAngle = getAngle();      // zero out sensor packet 20, "angle since last request"                             
} // end setup

void loop(){
  if(n < 1)
    Serial.println("base line, should be all zeros");
    

  float tempAngle = getAngle();
  
  //testEncoder(90);
  waitForButton();
  driveWheels(100, -100);

  turnCW(180);
  stopDrive();            // stop and wait

  //getAngle();

  //Serial.print("turn angle ");
  //Serial.println(turnAngle);
  
  //waitForButton();
  //testWheelDrop();
  n++;                  // count time through loop()
} // end loop

// ************** my functions *****************
void turnCW(int limit){
  float totalRotation = 0.0;
  float temp = getAngle();             // make sure angle sensor is zeroed out

  while(totalRotation <= limit){
    delay(15);
    Serial.print("total rotation since last update: ");
    Serial.println(totalRotation);
    totalRotation += getAngle();
  }
} // end turnCW

float getAngle(void){

  int i = 0;
  float rotation = 0.0;
  
  
  mySerial.write(142);
  mySerial.write(20);
  //delay(10);

// 2 bytes, angle high byte, angle low byte
  while(mySerial.available()){
    encoderData[i] = mySerial.read();
    //Serial.println("angle data read");
    i++;
  } // end while
/*
  for(int j = 0; j < i; j++){
    Serial.print("data byte # ");
    Serial.print(j);
    Serial.print("  ");
    Serial.println(encoderData[j]);
  }  // end for
*/
  
  //angleDegrees = (int)(encoderData[0] << 8)|(int)(encoderData[1]&0xFF);
  rotation = (int)word((int)encoderData[0], (int)encoderData[1]);
  rotation /= 0.324056; 
  //Serial.print("degrees turned = ");
  //Serial.println(angleDegrees);

   i = 0;
   return rotation;
} // end get Angle

void getInfo(void){
  if (mySerial.available())   //First order of business: listen to Roomba
    Serial.println(mySerial.read());   //writes to USB input from soft serial if connected to laptop serial monitor
  
} // end getInfo

void testEncoder(int turn){

  int i = 0;

// send request for 2 sensor packets
// 43 = left wheel encoder (highbyte, lowbyte)
// 44 = right whee encoder (highbyte, lowbyte)
  mySerial.write(149);
  mySerial.write(2);
  mySerial.write(43);
  mySerial.write(44);
  delay(100);
//getInfo();

// clear old sensor data

  for(int i = 0; i < 4; i++){
    encoderData[i] = 0;
  } // end for clear old sensor data
  
// read the 2 bytes for right and 2 bytes for left wheel encoude counts
  while(mySerial.available()){
    encoderData[i] = mySerial.read();
    Serial.println("data read");
    i++;
  } // end while

  for(int j = 0; j < i; j++){
    Serial.println(encoderData[j]);
  }
  // word() return unsigned int so it is necessary to cast it as an (int)
  rightEncoderDistance = (int)word((int)encoderData[2], (int)encoderData[3])&0xFF;
  leftEncoderDistance = (int)word((int)encoderData[0], (int)encoderData[1])&0xFF;
  
  //rightEncoderDistance = (int)(encoderData[2] << 8)|(int)(encoderData[3]&0xFF);
  //leftEncoderDistance = (int)(encoderData[0] << 8)|(int)(encoderData[1]&0xFF);

  //rightEncoderDistance = (int)(encoderData[2] << 8)|(int)(encoderData[3]);
  //leftEncoderDistance = (int)(encoderData[0] << 8)|(int)(encoderData[1]);
  
  Serial.print("right encoder = ");
  Serial.println(rightEncoderDistance);

  Serial.print("left encoder = ");
  Serial.println(leftEncoderDistance); 

  float rightDistance = rightEncoderDistance * 72 / 508.8 * PIE;
  float leftDistance = leftEncoderDistance * 72 / 508.8 * PIE;

  Serial.print("right distance = ");
  Serial.println(rightDistance);

  Serial.print("left distance = ");
  Serial.println(leftDistance);
  
   float radians=((rightDistance)-(leftDistance))/235;
   //float radians=((rightEncoderDistance*72*3.14/508.8)-(leftEncoderDistance*72*3.14/508.8))/235;
   float angle = radians * 180 / PIE;
   i = 0;
   Serial.print("turn angle ");
   Serial.println(angle);
} // end testEncoder

void testWheelDrop(void){
 
  mySerial.write(142);
  mySerial.write(9);
  delay(250);
  Serial.println(mySerial.read());
  if(mySerial.available()){
    inByte = mySerial.read();
    Serial.println(inByte);
  } // end if
} // end testWheelDrop
  
/************************************************
 * beep to indicate roomba is waiting for a button press
 */
 void playBeep(void){

   mySerial.write("\x8c\x01\x04\x42\x20\x3e\x20\x42\x20\x3e\x20"); // [140] [1] [4] [68] [32]
   mySerial.write("\x8d\x01"); // [141] [1] play it (in slot 1)
 
 // byte song3[5]={140, 3, 1, 64, 16};
  //byte playSong3[2]={141, 3};
  //mySerial.write(song3, 5);
  //mySerial.write(playSong3, 2);
/*
  mySerial.write(140);
  mySerial.write(3);
  mySerial.write(1);
  mySerial.write(64);
  mySerial.write(16);

  mySerial.write(141);
  mySerial.write(3);
*/  
  
 } // end playBeed

/**********************************************
 * do a little dance to signal that roomba is in safe mode
 * and is ready to start reciving and transmitting
 **********************************************/
 void readyDance(void){
  driveWheels(50, -50);
  delay(250);
  driveWheels(-50, 50);
  delay(500);
  driveWheels(50, -50);
  delay(250);
  stopDrive();
 } // end readyDance()

/********************************************
 * wait for button push to continue
 ********************************************/
void waitForButton(void){
  while(digitalRead(buttonPin)==HIGH){
    //Serial.print("button state: ");
    //Serial.println(digitalRead(buttonPin));
    //Serial.println("push button to proceed");
  } // end while button pin
} // end of waitForButton()

/*******************************************
 * wake up roomba by pulsing dd pin 5 high low high
 ********************************************/
void stopDrive(void){
  mySerial.write(byte(137));
  mySerial.write(byte(0));
  mySerial.write(byte(0));
  mySerial.write(byte(0));
  mySerial.write(byte(0));  
} // end stopdrive()

/***********************************************
 * direct drive wheel motors
 * 4 bytes are
 * right velocity high byte
 * right velocity low byte
 * left velocity high byte
 * left velocitn low byte
 * 
 * constrain is used to ensure that the parameter values are within 
 * range for driving roomba's wheels
 */
void driveWheels(int right, int left)
{
  constrain(right, -500, 500);
  constrain(left, -500, 500);

  //Serial.println("in driveWheels");
  
  mySerial.write(145);
  mySerial.write(right >> 8);
  mySerial.write(right);
  mySerial.write(left >> 8);
  mySerial.write(left);
 } // end drive wheels

 /* goForward sends the opcode 137 drive 
  *  the 4 bytes are
  *  velocity high byte, low byte
  *  radius high byte, low byte
  *  this op code doesn't drive the wheel motors directly
  */
 void goForward(void)
 {
  //Serial.println("in goForward");
    mySerial.write(byte(137));
    mySerial.write(byte(0));
    mySerial.write(byte(100));
    mySerial.write(byte(0x80));
    mySerial.write(byte (0));    // go forward
 } // end goForward
 
 void wakeUp (void)
{
  //setWarningLED(ON);
  //Serial.println("wake up, wake up.");
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(2000);
}
 

