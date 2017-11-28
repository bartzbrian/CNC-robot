//global vars
const int Xpulse = 6;
const int Xdirection = 7;
const int XlimitMin = 2;
const int XlimitMax = 3;
int XpulseLength = 0;

boolean measuringX = true;
int xDirection = 0;

// initialize with -1 to say it is still invalid 
int desiredPosition = -1;

int someCounterThatWillBeReplacedBySequencer = 0;

// example values only obviously it would be more than 100 pulses
int positions[9][2] = 
{ 
    // row 1
    {100,100},       
    {200,100},  
    {300,100},
  
    // row 2
    {100,200},
    {200,200},  
    {300,200},
  
    // row 3
    {100,300},   
    {200,300},
    {300,300},

};


//set pinModes, run calibration sequence
void setup() {
  pinMode(Xpulse,OUTPUT);
  pinMode(Xdirection,OUTPUT);
  pinMode(XlimitMin, INPUT);
  pinMode(XlimitMax, INPUT);
  //Serial.begin(9600);
  
  resetX();
}

void setDirectionX(int dir){
  xDirection = dir;
  digitalWrite(Xdirection,xDirection);
}

void resetX() {
  
  //set direction towards min
  setDirectionX(0);
  
  // block until limit reached
  while(digitalRead(XlimitMin) == 0){
    pulseX();
  }
}

// these have to be declared in the parent scope or it is always the initial value
// i don't know if you can initialize to micros() outside a function
// if not, initialize them in the setup
unsigned long previous_time_x = micros();
unsigned long current_time_x = micros();

// this is designed to be called from the main loop and non-blocking
int pulseX() {
  
  // safetyMin
  if( xDirection == 0 && digitalRead(XlimitMin) ){
    return -1;
  }
  
  // safetyMax
  if( xDirection == 1 && digitalRead(XlimitMax) ){
    return -1;
  }
  
  current_time = micros();
  unsigned long elapsed = current_time - previous_time;

  // 5 have elapsed
  if(elapsed <= 5){
      // high for about 5 microseconds (maybe use milliseconds if this is too fast)
      digitalWrite(Xpulse,HIGH);
  }

  // remember, while loops are blocking (it's an infinite loop)
  // so just do if in the main loop
  if(elapsed > 5){
      // low for about 5 milliseconds
      digitalWrite(Xpulse,LOW);  
  }

  // reset the counters
  if( elapsed > 10 ){
     previous_time = micros();
     // 1 pulse cycle is complete. it will now be 'counted'
     return 1;
  }

  return 0;
}


// moves to 1/3,1/2,then 2/3 the X axis' pulse length (determined from calibration) 
// in each direction on repeat, with 1 second delay between each movement. 
void loop() { 
  
  if( measuringX ){
    
    int pulseResult = pulseX();
    if( pulseResult >= 0 ){
       // measure ACTUAL pulses sent
       XpulseLength += pulseX();
    }
    
    // abort measuring since we hit a limit
    if( pulseResult == -1 ){
      measuringX = false;
      currentPositionX = XpulseLength;
      
      // TODO: make these editable/nudgeable
      positions[0][0] = Math.floor(XpulseLength * .3333);
      positions[1][0] = Math.floor(XpulseLength * .5);
      positions[2][0] = Math.floor(XpulseLength * .6666);
      // 2nd row
      positions[3][0] = Math.floor(XpulseLength * .3333);
      positions[4][0] = Math.floor(XpulseLength * .5);
      positions[5][0] = Math.floor(XpulseLength * .6666);
      // 3rd row
      positions[6][0] = Math.floor(XpulseLength * .3333);
      positions[7][0] = Math.floor(XpulseLength * .5);
      positions[8][0] = Math.floor(XpulseLength * .6666);
      
      // TODO: assign this with sequencing logic or buttons or something
      desiredPosition = 0;
    }
    
    // bail if we are still measuring
    return;
  } 
  
  // i don't know how long this should go until it sets new desired position
  if(someCounterThatWillBeReplacedBySequencer > 1000000000 && desiredPosition < 9) {
    someCounterThatWillBeReplacedBySequencer = 0;
    desiredPosition ++;
  }
  
  if(desiredPosition >= 0){    
    // goto the desired spot in the position array
    // for now it is just a huge number, and each time that huge number is hit
    // we tell the CNC to move to the next pair of x,y coordinates 
    gotoPosition(desiredPosition);
  }
  
  someCounterThatWillBeReplacedBySequencer ++;
}

gotoPosition(int pos){
  
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

