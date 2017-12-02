//global vars
const int Xpulse = 6;
const int Xdirection = 7;
const int XlimitMin = 2;
const int XlimitMax = 3;
long XpulseLength = 0;
int dirX = 0;
int prevLimitMax = 0;
long currentPositionX = -1;

long positions[3][2];
int positionCounter = 0; //this will be replaced by sequencing logic
int calculatingTargetPosition = 0;
int calculatingPositions = 1;
int movingToTarget = 0;
long targetPos = 0;

//set pinModes, run calibration sequence
void setup() {
  pinMode(Xpulse,OUTPUT);
  pinMode(Xdirection,OUTPUT);
  pinMode(XlimitMin, INPUT);
  pinMode(XlimitMax, INPUT);
  Serial.begin(9600);
  measureX();
  currentPositionX = XpulseLength;
}


void setDirectionX(int dir){
  if( dir != dirX){
    dirX = dir;
    digitalWrite(Xdirection,dirX);
  } 
}


// moves towards min until limit switch tripped, then moves towards max,
// counting pulses until second limit switch tripped in order to get the
// length of the whole X-axis actuator in pulses. Returns that length.
void measureX(){

    int tripCheck = 0;
    
    setDirectionX(0); //set direction towards min
    
    while(tripCheck != -1){    
      tripCheck = pulseX();
    }

    Serial.println("REACHED MIN, TURNING AROUND");
    setDirectionX(1); // turn around
    tripCheck = 0;// reset the tripCheck
    Serial.println("MOVING OTHER WAY");
    
    while(tripCheck != -1){   
      tripCheck = pulseX();
      XpulseLength += tripCheck;
    }
     
    Serial.print("MAX REACHED, FINAL X-AXIS PULSE COUNT: ");
    Serial.println(XpulseLength);
    
  }

//sends a pulse on the X-axis, returns one after a pulse cycle
int pulseX() {

    int maxX = 0;
    
    // safetyMin
    if( dirX == 0 && digitalRead(XlimitMin) ){ 
      return -1;
    } else if( dirX == 1 ){
      maxX = digitalRead(XlimitMax);
      if( maxX ==1 && prevLimitMax == 1 ){
        return -1;
      }
    }
    prevLimitMax = maxX;

    digitalWrite(Xpulse,HIGH);
    delayMicroseconds(5);
    digitalWrite(Xpulse,LOW);
    delayMicroseconds(5);
    return 1;

  }
  
//int gotoPosition(int pos){
//  
//}

void loop() { 
  
    if (calculatingPositions){
      
        positions[0][0] = (XpulseLength * .3333);
        positions[1][0] = (XpulseLength * .5);
        positions[2][0] = (XpulseLength * .6666);
  
        calculatingPositions = 0;
        Serial.println(String(positions[0][0]));
        calculatingTargetPosition = 1;
        
      }

    if (calculatingTargetPosition){
      
      if (positionCounter > 2){//resets the sequence for now
        positionCounter = 0;
        }
        
      targetPos = positions[positionCounter][0]; //set the new target position
      calculatingTargetPosition = 0; //causes this to not get 
      positionCounter++;
      movingToTarget = 1;
      
      if (targetPos > currentPositionX){
        setDirectionX(1);
      } else if (targetPos < currentPositionX){
        setDirectionX(0);
      }

      Serial.print("new target position is: ");
      Serial.print(targetPos);
      Serial.print(", current position is: ");
      Serial.print(currentPositionX);
      Serial.print(", direction is set to: ");
      Serial.println(dirX);
        
    }
    
   if (movingToTarget){
    
      if (targetPos == currentPositionX){
          calculatingTargetPosition = 1;
          movingToTarget = 0;
          Serial.println("done moving to this position");
          delay(1000);
        }

      int tripCheck = 0;
      tripCheck = pulseX();
      if (tripCheck == -1){
        Serial.println("switch tripped, aborting");
        movingToTarget = 0;
       } else {
        if (dirX == 1){
            currentPositionX++;
          }else if (dirX == 0){
            currentPositionX--;
            }
        }    
    }
    
  }//end loop
