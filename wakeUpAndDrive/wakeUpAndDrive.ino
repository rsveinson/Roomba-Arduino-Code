void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

} // end loop

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

/***********************************************
 * direct drive wheel motors
 * 4 bytes are
 * right velocity high byte
 * right velocity low byte
 * left velocity high byte
 * left velocitn low byte
 * 
 * examples of both bitwise shift right and highByte, lowByte
 * to parse the argument into 2 bytes
 * 
 * constrain is used to ensure that the parameter values are within 
 * range for driving roomba's wheels
 */
void driveWheels(int right, int left)
{
  constrain(right, -500, 500);
  constrain(left, -500, 500);

  Serial.println("in driveWheels");
  
  mySerial.write(145);
//  mySerial.write(right >> 8);
//  mySerial.write(right);
//  mySerial.write(left >> 8);
//  mySerial.write(left);

  mySerial.write(highByte(right));
  mySerial.write(lowByte(right));
  mySerial.write(highByte(left));
  mySerial.write(lowByte(left));
 } // end drive wheels
 
/*******************************************
 * stop roomba, send drive opcode (137) 
 * with all zero arguments
 ********************************************/
void stopDrive(void){
  mySerial.write(byte(137));
  mySerial.write(byte(0));
  mySerial.write(byte(0));
  mySerial.write(byte(0));
  mySerial.write(byte(0));  
} // end stopdrive()

/*******************************************
 * wake up roomba by pulsing dd pin 5 high low high
 ********************************************/
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
