//pin map
const int Xpulse = 6;
const int Xdirection = 7;
const int XlimitMin = 2;
const int XlimitMax = 3;
const int YlimitMin = 5;
const int YlimitMax = 4;
const int Ypulse = 8;
const int Ydirection = 9;

//measure vars X
long XpulseLength = 0;
int dirX = 0;
int prevLimitMaxX = 0;
long currentPositionX = -1;

//measure vars Y
long YpulseLength = 0;
int dirY = 0;
int prevLimitMaxY = 0;
int prevLimitMinY = 0;
long currentPositionY = -1;

//loop vars
long positions[9][2];
int positionCounter = 0; //this will be replaced by sequencing logic
int calculatingTargetPosition = 0;
int calculatingPositions = 1;
int movingToTarget = 0;
long targetPosX = 0;
long targetPosY = 0;


//set pinModes, run measurement sequences
void setup() {
  pinMode(Xpulse,OUTPUT);
  pinMode(Xdirection,OUTPUT);
  pinMode(XlimitMin, INPUT);
  pinMode(XlimitMax, INPUT);
  pinMode(Ypulse,OUTPUT);
  pinMode(Ydirection,OUTPUT);
  pinMode(YlimitMin, INPUT);
  pinMode(YlimitMax, INPUT);
  Serial.begin(9600);
  measureX();
  currentPositionX = 0;
  measureY();
  currentPositionY = 0;
}


void setDirectionX(int dir){
  if( dir != dirX){
    dirX = dir;
    digitalWrite(Xdirection,dirX);
  } 
}
void setDirectionY(int dir){
  if( dir != dirY){
    dirY = dir;
    digitalWrite(Ydirection,dirY);
  } 
}


// moves towards min until limit switch tripped, then moves towards max,
// counting pulses until second limit switch tripped in order to get the
// length of the whole X-axis actuator in pulses. Returns that length.
void measureX(){

    int tripCheck = 0;
    
    setDirectionX(1); //set direction towards min
    
    while(tripCheck != -1){    
      tripCheck = pulseX();
    }

    Serial.println("REACHED MAX, TURNING AROUND");
    setDirectionX(0); // turn around
    tripCheck = 0;// reset the tripCheck
    Serial.println("MOVING OTHER WAY");
    
    while(tripCheck != -1){   
      tripCheck = pulseX();
      XpulseLength += tripCheck;
    }
     
    Serial.print("MIN REACHED, FINAL X-AXIS PULSE COUNT: ");
    Serial.println(XpulseLength);    
  }

void measureY(){

    int tripCheck = 0;
    
    setDirectionY(1); //set direction towards min
    
    while(tripCheck != -1){  
      tripCheck = pulseY();
    }

    Serial.println("REACHED MAX, TURNING AROUND");
    setDirectionY(0); // turn around
    tripCheck = 0;// reset the tripCheck
    Serial.println("MOVING OTHER WAY");
    
    while(tripCheck != -1){   
      tripCheck = pulseY();
      YpulseLength += tripCheck;
    }
     
    Serial.print("MIN REACHED, FINAL Y-AXIS PULSE COUNT: ");
    Serial.println(YpulseLength);    
  }

//sends a pulse on the X-axis, returns one after a pulse cycle, minus 1 if limit tripped
int pulseX() {

    int maxX = 0;
    
    // safetyMin
    if( dirX == 0 && digitalRead(XlimitMin) ){ 
      return -1;
    } else if( dirX == 1 ){
      maxX = digitalRead(XlimitMax);
      if( maxX ==1 && prevLimitMaxX == 1 ){
        return -1;
      }
    }
    prevLimitMaxX = maxX;

    digitalWrite(Xpulse,HIGH);
    delayMicroseconds(5);
    digitalWrite(Xpulse,LOW);
    delayMicroseconds(5);
    return 1;

  }

int pulseY() {

    int maxY = 0;
    int minY = 0;
    // safetyMin
    if( dirY == 0){ 
      minY = digitalRead(YlimitMin);
      if( minY == 1 && prevLimitMinY == 1){
      return -1; 
      }
    } else if( dirY == 1 ){
      maxY = digitalRead(YlimitMax);
      if( maxY ==1 && prevLimitMaxY == 1 ){
        return -1;
      }
    }
    prevLimitMaxY = maxY;
    prevLimitMinY = minY;
    digitalWrite(Ypulse,HIGH);
    delayMicroseconds(5);
    digitalWrite(Ypulse,LOW);
    delayMicroseconds(5);
    return 1;

  }



void loop() { 
  
    if (calculatingPositions){

        //X
        positions[0][0] = (XpulseLength * .165);
        positions[1][0] = (XpulseLength * .5);
        positions[2][0] = (XpulseLength * .885);

        positions[3][0] = (XpulseLength * .165);
        positions[4][0] = (XpulseLength * .5);
        positions[5][0] = (XpulseLength * .885);

        positions[6][0] = (XpulseLength * .165);
        positions[7][0] = (XpulseLength * .5);
        positions[8][0] = (XpulseLength * .885);

        //Y
        positions[0][1] = (YpulseLength * .333);
        positions[1][1] = (YpulseLength * .333);
        positions[2][1] = (YpulseLength * .333);

        positions[3][1] = (YpulseLength * .6);
        positions[4][1] = (YpulseLength * .6);
        positions[5][1] = (YpulseLength * .6);

        positions[6][1] = (YpulseLength * .87);
        positions[7][1] = (YpulseLength * .87);
        positions[8][1] = (YpulseLength * .87);
  
        calculatingPositions = 0;
        Serial.println(String(positions[0][0]));
        calculatingTargetPosition = 1;
        
      }

    if (calculatingTargetPosition){
      
      if (positionCounter > 8){//resets the sequence for now
        positionCounter = 0;
        }
        
      targetPosX = positions[positionCounter][0]; //set the new target positions
      targetPosY = positions[positionCounter][1];
      calculatingTargetPosition = 0; //causes this if to only happen between moves 
      positionCounter++;
      movingToTarget = 1;
      
      //set DirX
      if (targetPosX > currentPositionX){
        setDirectionX(1);
      } else if (targetPosX < currentPositionX){
        setDirectionX(0);
      }

      //set DirY
      if (targetPosY > currentPositionY){
        setDirectionY(1);
      } else if (targetPosY < currentPositionY){
        setDirectionY(0);
      }

      Serial.print("new target position is: ");
      Serial.print(targetPosX);
      Serial.print(", ");
      Serial.print(targetPosY);
      Serial.print(", current position is: ");
      Serial.print(currentPositionX);
      Serial.print(", ");
      Serial.print(currentPositionY);
      Serial.print(", directions are set to: ");
      Serial.print(dirX);
      Serial.print(", ");
      Serial.println(dirY);
        
    }
    
   if (movingToTarget){

      // if in final location
      if (currentPositionX == targetPosX && currentPositionY == targetPosY){
          calculatingTargetPosition = 1;
          movingToTarget = 0;
          Serial.println("done moving to this position");
          delay(1000);
          //IMPLEMENT THE Z AXIS DRAWING GRABBER FUNCTION AND MAYBE CALL IT HERE
        }

      //pulse X until at position
      if(currentPositionX != targetPosX){
        int tripCheckX = 0;
        tripCheckX = pulseX();
        if (tripCheckX == -1){
          Serial.println("X switch tripped, aborting");
          movingToTarget = 0;
         } else {
          if (dirX == 1){
              currentPositionX++;
            }else if (dirX == 0){
              currentPositionX--;
              }
          }
      }

      //pulse Y until at position
      if(currentPositionY != targetPosY){
        int tripCheckY = 0;
        tripCheckY = pulseY();
        if (tripCheckY == -1){
          Serial.println("Y switch tripped, aborting");
          movingToTarget = 0;
         } else {
          if (dirY == 1){
              currentPositionY++;
            }else if (dirY == 0){
              currentPositionY--;
              }
          }
      }  
            
    }//end moving to target  
  }//end loop
