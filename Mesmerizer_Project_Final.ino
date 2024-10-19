#include <Servo.h>

//miku and teto angle limits
#define MIN_ANG 80
#define MAX_ANG 100
#define OFFSET 85

//absolute limits for servo motor
#define MIN_ABS 0
#define MAX_ABS 180
#define MID 90

//input pins
#define ON_IN 2  // all toggle switch
#define CC_IN 4  

//output pins
#define FOCUS_OUT 5
#define MIKU_BASE 3
#define MIKU_WRIST 6
#define TETO_BASE 9
#define TETO_WRIST 10
#define STAR_OUT 11

//servo name declaration
Servo mikuB;
Servo mikuW;
Servo tetoB;
Servo tetoW;
Servo star;

void setup() {
  //digital output for focus motor
  pinMode(FOCUS_OUT, OUTPUT);

  //assign servo pin
  mikuB.attach(MIKU_BASE);
  mikuW.attach(MIKU_WRIST);
  tetoB.attach(TETO_BASE);
  tetoW.attach(TETO_WRIST);
  star.attach(STAR_OUT);

  //button input
  pinMode(ON_IN, INPUT_PULLUP);
  pinMode(CC_IN, INPUT_PULLUP);

  //init
  star.write(MAX_ABS);
  mikuB.write(MID);
  mikuW.write(MID);
  tetoB.write(MID);
  tetoW.write(MID);
  analogWrite(FOCUS_OUT, 0);
  //Serial.begin(9600); trouble shooting purposes
}

void loop() {
  //incoming lazy coding

  //var
  unsigned char mBasePos;
  unsigned char mWristPos;
  unsigned char tBasePos;
  unsigned char tWristPos;

  static unsigned char basePos = MIN_ANG;
  static unsigned char wristPos;  //no need to assign anything to this
  static char jpDir = 0;
  static int starPos = MAX_ABS;
  static char starDir = 1;  // if 1 subtract else add
  static char flip = 0;     //

  //special var for miku
  static unsigned char osPos = MID;  //offset base position
  static char osDir = 0;

  // WHILE THE "POWER" IS ON AND THE SWTICH IS OFF
  // miku teto and the star should move
  while (!(digitalRead(ON_IN)) && (digitalRead(CC_IN))) {

    //this flip code is my attempt to slow the star speed
    flip ^= 1;

    if (flip) {
      starDir ? starPos-- : starPos++;
      star.write(starPos);
      if (starDir && (starPos < MIN_ABS)) starDir ^= 1;  // flip dir
      else if (!starDir && (starPos > MAX_ABS)) starDir ^= 1;
    }

    //move servo depending on the direction of the jump
    jpDir ? basePos-- : basePos += 2;
    wristPos = 180 - basePos;

    //assigning values
    tBasePos = basePos;
    tWristPos = wristPos;
    //miku is opposite
    mBasePos = wristPos;
    mWristPos = basePos;

    //sending command
    mikuB.write(mBasePos);
    tetoB.write(tBasePos);
    mikuW.write(mWristPos);
    tetoW.write(tWristPos);

    //would use timer instead but servo took all the good timers
    //and im lazy to find a better alternative to delay
    delay(10);

    //limit check and switch direction when it hits limit
    if (!jpDir && (basePos > MAX_ANG)) jpDir ^= 1;
    else if (jpDir && (basePos < MIN_ANG)) jpDir ^= 1;
  }

  // WHILE THE "POWER" IS ON AND THE SWTICH IS ON
  while (!(digitalRead(ON_IN)) && !(digitalRead(CC_IN))) {
    //turn on motor
    analogWrite(FOCUS_OUT, 230);  //200 stall

    //same thing as above without the flip
    starDir ? starPos-- : starPos++;
    star.write(starPos);
    if (starDir && (starPos < MIN_ABS)) starDir ^= 1;  // flip dir
    else if (!starDir && (starPos > MAX_ABS)) starDir ^= 1;

    jpDir ? basePos -= 5 : basePos += 10;

    wristPos = 180 - basePos;
    tBasePos = basePos;
    tWristPos = wristPos;

    //special jump for miku. the jump is erratic
    osDir ? osPos -= 5 : osPos += 10;

    mBasePos = 180 - osPos;
    mWristPos = osPos;

    //sending command
    mikuB.write(mBasePos);
    mikuW.write(mWristPos);
    tetoB.write(tBasePos);
    tetoW.write(tWristPos);

    //check limits
    if (!jpDir && (basePos > MAX_ANG)) jpDir ^= 1;
    else if (jpDir && (basePos < MIN_ANG)) jpDir ^= 1;
    //offset limits
    if (!osDir && (osPos > MAX_ANG)) osDir ^= 1;
    else if (osDir && (osPos < MIN_ANG)) osDir ^= 1;
    delay(37);
  }

  //if button is off reset all the position and turn off motor
  analogWrite(FOCUS_OUT, 0);
  mikuW.write(MID);
  mikuB.write(MID);
  tetoB.write(MID);
  tetoW.write(MID);
  star.write(MID);
}
