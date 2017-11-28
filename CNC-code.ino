//global vars
const int Xpulse = 6;
const int Xdirection = 7;
const int XlimitMin = 2;
const int XlimitMax = 3;
int XpulseLength = 0;

//set pinModes, run calibration sequence
void setup() {
  pinMode(Xpulse,OUTPUT);
  pinMode(Xdirection,OUTPUT);
  pinMode(XlimitMin, INPUT);
  pinMode(XlimitMax, INPUT);
  //Serial.begin(9600);
  XpulseLength = calibrationX();
}



// moves towards min until limit switch tripped, then moves towards max,
// counting pulses until second limit switch tripped in order to get the
// length of the whole X-axis actuator in pulses. Returns that length.
int calibrationX(){

    int pulseCount = 0;
    int reachedEnd = 0;
    
    digitalWrite(Xdirection,LOW); //set direction towards min
    
    while(reachedEnd == 0){
      
      pulseX();
      
      if (digitalRead(XlimitMin) == 1){ //if limit switch tripped
          reachedEnd = 1;
          digitalWrite(Xdirection,HIGH); //switch direction
          //Serial.println("MIN REACHED, CHANGING DIRECTION");
        }
    }
    
    reachedEnd = 0; //reset stop condition
    //Serial.println("MOVING OTHER WAY");

    int current = 0; 
    int previous = 0; 
    
    while(reachedEnd == 0){ // set direction towards max, this time counting pulses
          
      pulseX();
      pulseCount++;
      current = digitalRead(XlimitMax);
      
      if (current == 1 && previous == 1){   //if limit switch tripped
          reachedEnd = 1;
          //Serial.println("MAX REACHED");
        } else {
          previous = current; //keeps track of last two switch vals to account for noise on long wire
        }        
    }
     
    //Serial.print("FINAL X-AXIS PULSE COUNT: ");
    //Serial.println(pulseCount);
    return pulseCount;
  }



//sends a pulse on the X-axis without blocking
void pulseX() {
    
    unsigned long previous_time = micros();
    unsigned long current_time = micros();

    digitalWrite(Xpulse,HIGH);
    while(current_time - previous_time <= 5){
        current_time = micros();
      }
    digitalWrite(Xpulse,LOW);
    previous_time = micros();
    while(current_time - previous_time <= 5){
        current_time = micros();
      }
  }



// moves to 1/3,1/2,then 2/3 the X axis' pulse length (determined from calibration) 
// in each direction on repeat, with 1 second delay between each movement. 
void loop() { 
    digitalWrite(Xdirection,LOW); 
      for (int j = 0; j < (XpulseLength/3); j++){   
          pulseX();
        }
      delay(1000);
      for (int j = 0; j < (XpulseLength/2); j++){  
          pulseX();
        }
      delay(1000);
      for (int j = 0; j < (2*(XpulseLength/3)); j++){
          pulseX();
        }
      delay(1000);
        
    digitalWrite(Xdirection,HIGH); //turn around
      for (int j = 0; j < (XpulseLength/3); j++){
          pulseX();
        }
      delay(1000);
      for (int j = 0; j < (XpulseLength/2); j++){
          pulseX();
        }
      delay(1000);
      for (int j = 0; j < (2*(XpulseLength/3)); j++){
          pulseX();
        }
      delay(1000);
  }
