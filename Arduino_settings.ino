#include <string.h>         //<String> á windows <string.h> á Linux
#include <SoftwareSerial.h>

#define SCLK 2              // Yellow
#define SDIO 3              // Red
#define NCS 4               // Yellow + tied
#define NRESET 5            // Black

#define REG_PRODUCT_ID 0x00
#define REG_REVISION_ID 0x01
#define REG_MOTION 0x02
#define REG_DELTA_X 0x03
#define REG_DELTA_Y 0x04
#define REG_SQUAL 0x05
#define REG_BURST_MODE 0x63
#define REG_PIXEL_GRAB 0x0b

#define FRAME_LENGTH 225

char xd,yd,tmp;
byte frame[256];
long X = 0;
long Y = 0;
bool FirstSend = false;
long xpos = 0, ypos = 0;
double theta = 0;
int theta_int = 0; 
int theta_reference = 0;
long int lastTimeD = 0;
long int lastTimeA = 0;
long int lastPosTime = 0;

const double r = 112.5*20;  // [mouse units] radius of setup must be verified 230*20 gamli 1125*2 new
String cmd = "";               // command
String tmpString;         // for substring 

String after_turn = "";
bool turnflag = false;

// RX is digital pin 10
// (connect to TX of other device - iRobot DB25 pin 2)
// TX is digital pin 11
// (connect to RX of other device - iRobot DB25 pin 1)
#define rxPin 10
#define txPin 11

int inByteH; // High byte from the roomba, in most cases we cannot use it
int inByteL; // Low byte from the roomba, this gives us the value
int value;
int distance = 0; // Distance from the roomba odometry
int angle = 0; // Angle input from the command string
int ang = 0; // Angle from the roomba odometry
int disPack = 19; // The Packet for distance data from roomba
int angPack = 20; // The Packet for angle data from roomba


// Set up a new software serial port:
SoftwareSerial softSerial = SoftwareSerial(rxPin, txPin);



void setup() {

  
  Serial.begin(9600); // Serial monitor works on Baud rate 9600
  

  reset(); // Initialize for the optical sensor


  byte productId = readRegister(REG_PRODUCT_ID);
  byte revisionId = readRegister(REG_REVISION_ID);

  delay(2000); // NEEDED!! To let the robot initialize

  pinMode(rxPin, INPUT); // Define RX pin
  pinMode(txPin, OUTPUT);// Define TX pin

  // Set the data rate for the SoftwareSerial port, this is the
  // iRobot's default rate of 57600 baud:
  softSerial.begin(57600);

  softSerial.write(128);  // This command starts the OI. You must
                          // always send the Start command before
                          // sending any other commands to the OI

  softSerial.write(131);  // safe mode


  ypos = 300*20;          // shift
}

void loop() {

  // yd is roomba forward
  // xd is roomba CCW
  dumpDelta(xd,yd);

  if (abs(xd) > 0)
    theta += (xd + 0.78)/r;                          // rad
  //Serial.println(X);
  xpos += - yd*sin(theta);
  ypos += yd*cos(theta);
  theta_int = 1000*theta;                 // rad*1000


  while( Serial.available() ){            // Read serial buffer
    tmp = Serial.read();
    cmd += tmp;
    delay(1);
  }

  if(turnflag == true && cmd[0] > NULL){
    after_turn = cmd;
    cmd = "";
  }


  if(cmd[0] == 'X' && cmd[1] == 'Y'  && cmd[2] == 'A' ){  // command string is "XYA" 
                                                          // and gives us X and Y coordinates and angle.

    Serial.print('#');                // Start line feed
    Serial.print(String(xpos/20));
    Serial.print('#');                
    Serial.print(String(ypos/20));
    Serial.print('#');                
    Serial.print(String(theta_int));      
    Serial.print('\n');               // this needs to be done for the Linux system does not use /r like windows 
    lastPosTime = millis();
    FirstSend = true;
                                      // and arduino newline
  }

  if((millis() - lastPosTime) > 900 && FirstSend)
  {
      Serial.print('#');                // Start line feed
    Serial.print(String(xpos/20));
    Serial.print('#');                
    Serial.print(String(ypos/20));
    Serial.print('#');                
    Serial.print(String(theta_int));      
    Serial.print('\n');               // this needs to be done for the Linux system does not use /r like windows 
    lastPosTime = millis();
  }

  if(cmd[0] == 'R' && cmd[1] == 'S'  && cmd[2] == 'T' ){    // command string is "RST" 
                                                            // Reset all values with command
    theta = 0;
    theta_int = 0; 
    theta_reference = 0;
    xpos = 0;
    ypos = 1125*2;
    angle = 0;
    ang = 0;
    distance = 0;
  }
  if(cmd[0] == 'D' && cmd[1] == 'F'  && cmd[2] == 'W' ){ // command string is "DFW" 
    goForward(50);                                       // Drive straight forward 50 mm/sek
  }
  if(cmd[0] == 'D' && cmd[1] == 'B'  && cmd[2] == 'W' ){ // command string is "DBW" 
    goBackward();                                        //  Drive straight backward 50 mm/sek
  }
  if(cmd[0] == 'S' && cmd[1] == 'T'  && cmd[2] == 'P' ){ // command string is "STP" 
    goForward(0);                                        // Stop
  }
  if(cmd[0] == 'T' && cmd[1] == 'N'  && cmd[2] == 'R' ){ // command string is "TNR" 
    turnR();                                             // Turn to right continuously
  }
  if(cmd[0] == 'T' && cmd[1] == 'N'  && cmd[2] == 'L' ){ // command string is "TNL" 
    turnL();                                             // Turn to left continuously
  }
  if(cmd[0] == 'T' && (cmd[1] == 'L' || cmd[1] == 'R' )  && cmd[2] == 'X' ){ // T(L/R)X***
                                                                             // command string is "TLX***" or 
                                                                             // "TRX***0" where *** is the
                                                                             // desirable angle in degrees

                                                                             // Turns left or right *** degees
    turnflag = true;                          // Notify that the create is turning this might take awhile
    tmpString = cmd.substring(3);             // Isolate the numbers
    angle = tmpString.toInt();                // Degrees to rotate
    angle = angle*17;                         // Convert degrees to rad*1000
    theta_reference = theta_int;              // set a reference point 

    if (angle != 0){                          // catch TLX000
      if (cmd[1] == 'R')
        turnR();                              // Start turning right
      if (cmd[1] == 'L')
        turnL();                              // Start turning right
    }
  }


 cmd = "";          // Clear command string

  // Stopping TLX and TRX
  if(angle != 0 && (angle - abs(theta_reference - theta_int) ) < 0  ){ // | óskgildi | - | delta(theta) |  < 0 rad
    goForward(0); // Stop
    angle = 0;
    turnflag = false;
    cmd = after_turn;
    after_turn = "";
  }



}

void dumpFrame(){
    byte frame[FRAME_LENGTH];
        
    for(int idx = 0; idx < FRAME_LENGTH; idx++){
      frame[idx] = readRegister(REG_PIXEL_GRAB);
      
      if((frame[idx] & 0x80) != 0x80)
        idx--;
      else 
        frame[idx] &= 0x7F;

      // todo - check if is valid
      Serial.println(frame[idx], DEC);
    }
    
    Serial.println("--------"); 
    Serial.println("--------"); 
}

void dumpDelta(char &x,char &y) {
  char motion = readRegister(REG_MOTION); // Freezes DX and DY until they are read or MOTION is read again.
  char dx = readRegister(REG_DELTA_X);
  char dy = readRegister(REG_DELTA_Y);
  x=dx;
  y=dy;
 ;  
}


void reset() {
  pinMode(SCLK, OUTPUT);
  pinMode(SDIO, INPUT);
  pinMode(NCS, OUTPUT);
  pinMode(NRESET, OUTPUT);
    
  digitalWrite(SCLK, LOW);
  digitalWrite(NCS, LOW);
  digitalWrite(NRESET, HIGH);
  delayMicroseconds(100);
  
  // Initiate chip reset
  digitalWrite(NRESET, LOW);
  pushByte(0xfa);
  pushByte(0x5a);
  digitalWrite(NRESET, HIGH);
  
  // Set 1000cpi resolution
  digitalWrite(NRESET, LOW);
  pushByte(0x0d);
  //pushByte(0x01); // 1000cpi
  pushByte(0x00); // 500cpi
  digitalWrite(NRESET, HIGH);
}

byte pullByte() {
  pinMode (SDIO, INPUT);

  delayMicroseconds(100); // tHOLD = 100us min.
  
  byte res = 0;
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK, LOW);
    res |= i * digitalRead (SDIO);
    delayMicroseconds(100);
    digitalWrite (SCLK, HIGH);
  }

  return res;
}

void pushByte(byte data){
  pinMode (SDIO, OUTPUT);
  
  delayMicroseconds(100); // tHOLD = 100us min.
  
  for (byte i=128; i >0 ; i >>= 1) {
    digitalWrite (SCLK, LOW);
    digitalWrite (SDIO, (data & i) != 0 ? HIGH : LOW);
    delayMicroseconds(100);
    digitalWrite (SCLK, HIGH);
  }
}

byte readRegister(byte address) {
  address &= 0x7F; // MSB indicates read mode: 0
  
  pushByte(address);
  
  byte data = pullByte();
  
  return data;  
}

void writeRegister(byte address, byte data) {
  address |= 0x80; // MSB indicates write mode: 1
  
  pushByte(address);
  
  delayMicroseconds(100);
  
  pushByte(data);

  delayMicroseconds(100); // tSWW, tSWR = 100us min.
}



void goForward(int v) {
 softSerial.write(137); // Opcode number for DRIVE
                        // Velocity (0 – 500 mm/s)
 softSerial.write((byte)0); //
 softSerial.write((byte)v); // Set velocity

 // Special case: straight = 32768 or 32767 = hex 8000 or 7FFF
 softSerial.write((byte)128); // hex 1000-0000-0000-0000 = 0x8000   0111 1111 1111 1111
 softSerial.write((byte)0); // hex 00
}

void goBackward(){
  softSerial.write(137);  // Opcode number for DRIVE
                          // Velocity must be put in hex
  softSerial.write((byte)0xff);   // 0xFFCE == 50
  softSerial.write(0xce);
  softSerial.write((byte)128); // hex 1000-0000-0000-0000 = 0x8000   0111 1111 1111 1111
  softSerial.write((byte)0); // hex 00
}

void turnL(){
  softSerial.write(137);

  // Velocity (0 - 500 mm/s)
  softSerial.write((byte)0);
  softSerial.write((byte)50); // v=50

  // Radius set as 0 to turn on the spot
  softSerial.write((byte)0);
  softSerial.write((byte)0);
}

void turnR(){
  softSerial.write(137);  // Opcode number for DRIVE
                          // Velocity must be in hex here

  softSerial.write((byte)0xff);   // 0xFFCE == 50
  softSerial.write(0xce);

  // Radius set as 0 to turn on the spot
  softSerial.write((byte)0);
  softSerial.write((byte)0); // r = 0
}

void getDist(){
// Ths function gets the distance from the roomba, however it is not used in this script

 softSerial.write((byte)142); // requests the OI to send a packet of
                              // sensor data bytes
 softSerial.write((byte)19); // request distance sensor value specifically
 delay(40); // poll sensor every 40 ms
 if (softSerial.available() > 0 && (millis()-lastTimeD>40)) {
  lastTimeD = millis();
  inByteH = softSerial.read();
  inByteL = softSerial.read();
  distance += inByteL;
 }
}

void getAng(){
  // Ths function gets the angle from the roomba, however it is not used in this script
 softSerial.write((byte)142); // requests the OI to send a packet of
 // sensor data bytes
 softSerial.write((byte)20); // request angle sensor value specifically
 // poll sensor every 40 ms
 if (softSerial.available() > 0 && (millis()-lastTimeA>40)) {
  lastTimeA = millis();
  inByteH = softSerial.read();
  inByteL = softSerial.read();
  if (inByteL < 50) // filter bad data
    ang += inByteL;
 }
}






