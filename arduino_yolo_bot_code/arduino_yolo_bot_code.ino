#include <Servo.h>

#include <SoftwareSerial.h>

#include "Arduino.h"

#define buffer 64
Servo servopan;
Servo servotilt;
SoftwareSerial serialcon(10,11);
char indata [buffer];
char inchar = -1;
int count =0 ;
int i=0;
int x1=0;
int y1=0;
int si1=0;
int m =0;

char xcor[buffer];
char ycor[buffer];
char sicor[buffer];

//****************************************************
// Motor Controllers
// ****************************************************

#define pwmA 3
#define dirA 2

#define pwmB 5
#define dirB 9

#define pwmC 6
#define dirC 7

int data =0;

typedef struct{
  int pulse;
  bool direction;
} MotorValues;

MotorValues motorA;
MotorValues motorB;
MotorValues motorC;

struct comm{
  int x;
  int y;
  int si;
  };




// ****************************************************
// Stops the motors
// RETURNS: none
// ****************************************************
void allStop() {
 analogWrite(pwmA, 0);
 analogWrite(pwmB, 0);
 analogWrite(pwmC, 0);
}

// ****************************************************
// Sets the PWM motor values
// RETURNS: none
// ****************************************************
void commandMotors() {
 analogWrite(pwmA, motorA.pulse);
 analogWrite(pwmB, motorB.pulse);
 analogWrite(pwmC, motorC.pulse);
}

// ****************************************************
// Forward motor movement
// RETURNS: none
// ****************************************************
void forwardMovement(int a) {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < a; i++)
  { motorA.pulse = 0; motorB.pulse = 150; motorC.pulse = 150; commandMotors(); delay(25); }  
  allStop();
}

// ****************************************************
// Backward motor movement
// RETURNS: none
// ****************************************************
void backwardMovement(int a) {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < a; i++)
  { motorA.pulse = 0; motorB.pulse = 150; motorC.pulse = 150; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Right motor movement
// RETURNS: none
// ****************************************************
void rightMovement (int a) {  
  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < a; i++)
  { motorA.pulse = 100; motorB.pulse = 60; motorC.pulse = 60; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Left motor movement
// RETURNS: none
// ****************************************************
void leftMovement (int a) {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < a; i++)
  { motorA.pulse = 100; motorB.pulse = 60; motorC.pulse = 60; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Spin Clowise motor movement
// RETURNS: none
// ****************************************************
void spinClockwise(int a) {  
  // Set motor directions
  digitalWrite(dirA, HIGH); digitalWrite(dirB, HIGH); digitalWrite(dirC, HIGH);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < a; i++)
  { motorA.pulse = 100; motorB.pulse = 100; motorC.pulse = 100; commandMotors(); delay(25); }
  allStop();
}

// ****************************************************
// Spin Counter Clockwise motor movement
// RETURNS: none
// ****************************************************
void spinCounterClockwise () {
  // Set motor directions
  digitalWrite(dirA, LOW); digitalWrite(dirB, LOW); digitalWrite(dirC, LOW);
  
  // Ramp up the appropriate motors
  for (int i = 0; i < 64; i++)
  { motorA.pulse = i; motorB.pulse = i; motorC.pulse = i; commandMotors(); delay(25); }
  allStop();
}

void setup(){
  Serial.begin(9600);
  serialcon.begin(9600);
  serialcon.println("ready");
  Serial.println("started");
  servopan.attach(12);
  servotilt.attach(13);


  pinMode(dirA, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(dirB, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(dirC, OUTPUT);
  pinMode(pwmC, OUTPUT);
  
  // Command all motors to stop
  allStop();
}




struct comm serial_com()
{
  struct comm j;
  byte bycount = serialcon.available();
  int dec =0;
  int dec1 =0;
  int dec2 =0;

  while(bycount < 9)
  {
   bycount = serialcon.available(); 
   }
    serialcon.println("hello from blue"+String(count));
   
    Serial.println("incomming data");
    int firstby = bycount;
    int remainby = 0;
    if (firstby >= buffer - 1){
      remainby = bycount-(buffer-1);
    }
    for (i =0; i<firstby; i++){
      inchar = serialcon.read();
      indata[i] = inchar;
    }
    indata[i]='\0';
    for(i=0;i<remainby;i++){
      inchar =serialcon.read();
    }

    for(i =0; i<3 ;i++)
    {
     xcor[i] = indata[i];
    }

    for(i =3; i<6 ;i++)
    {
     ycor[i -3] = indata[i];
    }

    for(i =6; i<10 ;i++)
    {
     sicor[i-6] = indata[i];
    }
    int l = strlen(xcor);
    for(i= 0; i < l; i++)
    {
      dec = dec*10 +(xcor[i] - '0');     
     } 

     l = strlen(ycor);
    for(i= 0; i < l; i++)
    {
      dec1 = dec1*10 +(ycor[i] - '0');     
     } 

     l = strlen(sicor);
    for(i= 0; i < l; i++)
    {
      dec2 = dec2*10 +(sicor[i] - '0');     
     } 
   
    
    //Serial.println(inchar);
    Serial.println(indata);
    for(i = 0; i >= bycount; i++)
    {
     indata[i]= 0;
    }
    Serial.println(indata);    
    count++;
  
  delay(100);
  j.x = dec;
  j.y = dec1;
  j.si = dec2;

  return j;
 
}


void loop(){
  struct comm j;
  
  j = serial_com();
  int k;
  x1 = j.x; // x coordinate received through bluetooth 
  y1 = j.y; // y coordinate received through bluettoth
  si1 = j.si; // size of the basketball received through bluetooth
  Serial.println(x1);
  
  Serial.println(y1);
  
  Serial.println(si1);

  if(x1 > 330 && y1 > 242)
  {
   k = x1 - 330;
   m = y1 - 242;
   k = k/30;
   m = m/5.2;
   k = (k * 10) + 90;
     m = m + 145;
   servopan.write(k);
   servotilt.write(m);
  }
  else if(x1 > 330 && y1 < 242)
  {
   k = x1 - 330;
   m =242 -y1;
   k = k/30;
   m = m/5.2;
   k = (k * 10) + 90;
   m = 145 - m;
   servopan.write(k);
   servotilt.write(m) ;
  }
  else if (x1 < 330 && y1 > 242)
  {
   k = 330 - x1;
   m = y1 - 242;
   k = k/30;
   m = m/5.2;
   k = 90- (k*10);
   m = m + 145;
   servopan.write(k);
   servotilt.write(m); 
  }
  else if(x1 < 330 && y1 <242)
  {
   k = 330 - x1;
   m = 242 - y1;
   k = k/30;
   m = m/5.2;
   k = 90- (k*10);
   m = 145 - m;
   servopan.write(k);
   servotilt.write(m); 
   }
   else
   {
    servopan.write(k);
    servotilt.write(m); 
   }  

   if(si1 > 70 && m >130 && k > 80 && k <100)
   {
    Serial.println("botback");
    backwardMovement(50); 
   }
   else if(k < 80 )
   {
     Serial.println("botright");
     rightMovement(50);
   }
   else if(k >100)
   {
    Serial.println("botleft");
    leftMovement(50);
   }
   else if(m < 120 && si1 <70 && k > 80 && k <100)
   {
    Serial.println("botforward");
    forwardMovement(50);
   }
   else 
   {
    Serial.println("botstop");
    allStop();
   }
   
}
