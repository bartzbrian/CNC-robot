//global vars
const int Xpulse = 6;
const int Xdirection = 7;
const int XlimitMin = 2;
const int XlimitMax = 3;
long XpulseLength = 0;
int currentPositionX = -1;

unsigned long previous_time_x = 0;
unsigned long current_time_x = 0;

boolean measuringX = true;
boolean resettingX = true;
int xDirection = 0;

// initialize with -1 to say it is still invalid 
int desiredPosition = -1;

int someCounterThatWillBeReplacedBySequencer = 0;

// example values only obviously it would be more than 100 pulses
int positions[9][2];
//= 
//{ 
//    // row 1
//    {100,100},       
//    {200,100},  
//    {300,100},
//  
//    // row 2
//    {100,200},
//    {200,200},  
//    {300,200},
//  
//    // row 3
//    {100,300},   
//    {200,300},
//    {300,300},
//
//};


//set pinModes, run calibration sequence
void setup() {
  Serial.begin(9600);
  Serial.println("starting");
  pinMode(Xpulse,OUTPUT);
  pinMode(Xdirection,OUTPUT);
  pinMode(XlimitMin, INPUT);
  pinMode(XlimitMax, INPUT);

  previous_time_x = micros();
  current_time_x = micros();
  
  // resetX();
  setDirectionX(0);
}

void setDirectionX(int dir){
  xDirection = dir;
  digitalWrite(Xdirection,xDirection);
}

void resetX() {
  
  //set direction towards min
  // setDirectionX(0);
  
  // block until limit reached
  // while(digitalRead(XlimitMin) == 0){
    pulseX();
    // delayMicroseconds(1);
  // }
}

int prevLimitMax = 0;

// this is designed to be called from the main loop and non-blocking
int pulseX() {

  int maxX = 0;
  
  
  // safetyMin
  if( xDirection == 0 && digitalRead(XlimitMin) ){
    return -1;
  } else if( xDirection == 1 ){
    maxX = digitalRead(XlimitMax);
    if( maxX ==1 && prevLimitMax == 1 ){
      return -1;
    }
  }

  prevLimitMax = maxX;
  
  current_time_x = micros();
  int elapsed = current_time_x - previous_time_x;
  
  if(elapsed <= 25){
      digitalWrite(Xpulse,HIGH);
  }

  // remember, while loops are blocking (it's an infinite loop)
  // so just do if in the main loop
  if(elapsed > 25 && elapsed < 50){
      digitalWrite(Xpulse,LOW);  
  }

  // reset the counters
  if( elapsed > 50 ){
     previous_time_x = micros();
     current_time_x = micros();
     return 1;
  }

  return 0;
}

int t = 0;
void loggit(String value){
  if( t % 1 == 0){
     Serial.println(value);
  }
 

  t++;
}


// moves to 1/3,1/2,then 2/3 the X axis' pulse length (determined from calibration) 
// in each direction on repeat, with 1 second delay between each movement. 
void loop() { 

  if( resettingX ){

    int result = pulseX();

    if( result == -1){
      resettingX = false;
      setDirectionX(1);
    }

    return;
  }

  if( measuringX ){

    
    int pulseResult = pulseX();
    
    if( pulseResult >= 0 ){
       // measure ACTUAL pulses sent
       XpulseLength += pulseX();
    }

    
    // abort measuring since we hit a limit
    if( pulseResult == -1 ){
      // this will abort the measuring condition in the next loop
      measuringX = false;
      // the head is now at the max pulse length
      currentPositionX = XpulseLength;
      
      // TODO: make these editable/nudgeable
      positions[0][0] = round(XpulseLength * .3333);
      positions[1][0] = floor(XpulseLength * .5);
      positions[2][0] = floor(XpulseLength * .6666);
      // 2nd row
      positions[3][0] = floor(XpulseLength * .3333);
      positions[4][0] = floor(XpulseLength * .5);
      positions[5][0] = floor(XpulseLength * .6666);
      // 3rd row
      positions[6][0] = floor(XpulseLength * .3333);
      positions[7][0] = floor(XpulseLength * .5);
      positions[8][0] = floor(XpulseLength * .6666);
      
      // TODO: assign this with sequencing logic or buttons or something
      desiredPosition = 0;

      Serial.println("Measured : " + String(XpulseLength)  +  " " + String(positions[0][0]));
    }
    
    // bail if we are still measuring
    return;
  } 
  
  // i don't know how long this should go until it sets new desired position
//  if(someCounterThatWillBeReplacedBySequencer > 1000000000 && desiredPosition < 9) {
//    someCounterThatWillBeReplacedBySequencer = 0;
//    desiredPosition ++;
//  }
  
  if(desiredPosition >= 0){    
    // goto the desired spot in the position array
    // for now it is just a huge number, and each time that huge number is hit
    // we tell the CNC to move to the next pair of x,y coordinates 
    gotoPosition(desiredPosition);
  }
  
  someCounterThatWillBeReplacedBySequencer ++;
}

void gotoPosition(int pos){
  
  int targetX = positions[pos][0];
  int targetY = positions[pos][1];
  
  // naive way for now. it might vibrate. i'll see if you can come up with a quick way to unvibrate it.
  if(currentPositionX > targetX){
    setDirectionX(0);
    // decrement the position by pulse result
    currentPositionX -= pulseX();
  }
  if(currentPositionX < targetX){
    setDirectionX(1);
    // increment the position by pulse result
    currentPositionX += pulseX();
  }
}

