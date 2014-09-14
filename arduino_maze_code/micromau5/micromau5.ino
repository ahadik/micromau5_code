#include <QueueList.h>
#include <Encoder.h>
#include <Dispatch.h>
#include <StackList.h>
#include "Wire.h"
#include "L3G.h"

//Include custom data types
#include "coord.h"
#include "entry.h"
#include "instruction.h"


//Navigation info
QueueList<instruction> instructions;

//Encoder Variables
const int R_encoder_A = 2;
const int R_encoder_B = 11;
const int L_encoder_A = 3;
const int L_encoder_B  = 12;

const int R_motor_forward = 5;
const int R_motor_backward = 9;
const int L_motor_forward = 6;
const int L_motor_backward = 10;
//Encoder instantiation
Encoder rightEnc(R_encoder_A, R_encoder_B);
Encoder leftEnc(L_encoder_A, L_encoder_B);
long positionRight = 0;
long positionLeft = 0;
float oneTurn = 2048L;

int motorSpeed = 0;
int actSpeed = 0;

// PID controls
int damping = 0.1;
int timerCount = 0;

//IR Variable Setup
const int ir_side_out = 4;
const int ir_front_out = 8;
int ir_diag_out = 7;


const int front_r_in = A0;//confirmed
const int front_l_in = A1;//confirmed
const int side_r_in = A2;
const int side_l_in = A3;//confirmed
const int diag_r_in = A6;//confirmed
const int diag_l_in = A7;//confirmed

byte globalHeading = 4;
coord globalCoord = {0,0};
coord globalEnd = {0,0};


Dispatch dispatch(ir_diag_out, ir_side_out, ir_front_out, front_r_in, front_l_in, side_r_in, side_l_in, diag_r_in, diag_l_in);

  //Gyro shit
  L3G gyro;
  
  //Define some global constants
  #define X 16
  #define Y 16
    
  entry maze[Y][X];
  //N,S,E,W
  byte headings[] = {1,2,4,8};
  unsigned long time; 
  int sampleTime=10; 
  int rate;
  
  int sampleNum=750;
    
  long dc_offset=0; 
  double noise=0;
  int prev_rate=0; 
  double angle=0; 
  
  //Sensor reading
  int sensorState = 0;


void setup(){
  Serial.begin(115200);
   dispatch.calibrateSensors();
   
  Serial.println(dispatch.calibratedArray[0]);
  Serial.println(dispatch.calibratedArray[1]);
  Serial.println(dispatch.calibratedArray[2]);
  Serial.println(dispatch.calibratedArray[3]);
  Serial.println(dispatch.calibratedArray[4]);
  Serial.println(dispatch.calibratedArray[5]);
  Serial.println();
  
  //Set IO for motors
  pinMode(R_encoder_A, INPUT);
  digitalWrite(R_encoder_A, LOW);
  pinMode(R_encoder_B, INPUT);
  digitalWrite(R_encoder_B, LOW);
  pinMode(L_encoder_A, INPUT);
  digitalWrite(L_encoder_A, LOW);
  pinMode(L_encoder_B, INPUT);
  digitalWrite(L_encoder_B, LOW);
  pinMode(R_motor_forward, OUTPUT);
  digitalWrite(R_motor_forward, LOW);
  pinMode(R_motor_backward, OUTPUT);
  digitalWrite(R_motor_backward, LOW);
  pinMode(L_motor_forward, OUTPUT);
  digitalWrite(L_motor_forward, LOW);
  pinMode(L_motor_backward, OUTPUT);
  digitalWrite(L_motor_backward, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(12, LOW);
  
 
  Wire.begin();
  
  while (!gyro.init());
  gyro.enableDefault();
  
  //Calculate initial DC offset and noise level
  for(int n=0;n<sampleNum;n++){ 
   gyro.read(); 
   dc_offset+=(long)gyro.g.z; 
  } 
  dc_offset=dc_offset/sampleNum; 
  
  for(int n=0;n<sampleNum;n++){
   gyro.read(); 

   if((int)gyro.g.z-dc_offset>noise) 
     noise=(int)gyro.g.z-dc_offset; 
    else if((int)gyro.g.z-dc_offset<-noise) 
     noise=-(int)gyro.g.z-dc_offset; 
  } 
  noise=noise/100; //gyro returns hundredths of degrees/sec 

  instantiate();

  printMaze();
 
 // Enable Timer2 interrupt.
 TIMSK2 = (0<<OCIE2A) | (1<<TOIE2);
}



ISR(TIMER2_OVF_vect) {
  // Interrupt routine.
  if(sensorState==0){
    //Power down the side sensors and power up the front LEDs
    dispatch.powerDown(dispatch.irArray[2][0]);
    dispatch.powerUp(dispatch.irArray[0][0]);
    sensorState++;
  }else if(sensorState==1){
    //Read both front sensors
    dispatch.readSensor(0,1);
    dispatch.readSensor(0,2);
    sensorState++;
  }else if(sensorState==2){
    //Power down the front sensors and power up the diag LEDs
    dispatch.powerDown(dispatch.irArray[0][0]);
    dispatch.powerUp(dispatch.irArray[1][0]);
    sensorState++;
  }else if(sensorState==3){
    //Read both diag sensors
    dispatch.readSensor(1,1);
    dispatch.readSensor(1,2);
    sensorState++;    
  }else if(sensorState==4){
    //Power down the diag sensors and power up the side LEDs
    dispatch.powerDown(dispatch.irArray[1][0]);
    dispatch.powerUp(dispatch.irArray[2][0]);
    sensorState++;
  }else if(sensorState==5){
    //Read both side sensors
    dispatch.readSensor(2,1);
    dispatch.readSensor(2,2);
    sensorState=0;
  }
}


byte readCurrent(){
  byte wallReading = 15;
  byte north = 0;
  byte south = 0;
  byte east = 0;
  byte west = 0;
  switch(globalHeading){
    case 1:
      //if the forward sensor is tripped
      if(dispatch.irValues[0]>100){
        //set north to 1
        north = 1;
      }
      //if the right sensor is tripped
      if(dispatch.irValues[4]>100){
        //set east to 4
        east = 4;
      }
      //if the left sensor is tripped
      if(dispatch.irValues[5]>100){
        //set west to 9
        west = 8;
      }
      //Subtract the sum of north east and west from the value of wall reading
      wallReading -= (north+east+west);
      break;
    case 2:
      //if the forward sensor is tripped
      if(dispatch.irValues[0]>100){
        //set south to 2
        south = 2;
      }
      //if the right sensor is tripped
      if(dispatch.irValues[4]>100){
        //set west to 8
        west = 8;
      }
      //if the left sensor is tripped
      if(dispatch.irValues[5]>100){
        //set east to 4       
        east = 4;
      }
      //subtract the sum from 15
      wallReading-=(south+east+west);
      break;
    case 4:
      //if the forward sensor is tripped
      if(dispatch.irValues[0]>100){
        //set east to 4
        east = 4;
      }
      //if the right sensor is tripped
      if(dispatch.irValues[4]>100){
        //set south to 2
        south = 2;
      }
      //if the left sensor is tripped
      if(dispatch.irValues[5]>100){
        //set north to 1
        north = 1;
      }
      //subtract the sum from 15
      wallReading-=(north+south+east);
      break;
    case 8:
      //if the forward sensor is tripped
      if(dispatch.irValues[0]>100){
        //set east to 8
        west = 8;
      }
      //if the right sensor is tripped
      if(dispatch.irValues[4]>100){
        //set north to 1
        north = 1;
      }
      //if the left sensor is tripped
      if(dispatch.irValues[5]>100){
        //set south to 1
        south = 2;
      }
      //subtract the sum from 15
      wallReading-=(west+north+south);
      break;
    }
    return wallReading;
  }

void gyroRead(){
  
  // Every 10 ms take a sample from the gyro 
 //if(millis() - time > sampleTime) {
   gyro.read(); 
   rate=((int)gyro.g.z-dc_offset)/100; 
    
    angle += ((double)(prev_rate + rate) * sampleTime) / 2000; 
    // remember the current speed for the next loop rate integration. 
    prev_rate = rate; 
    // Keep our angle between 0-359 degrees 
    
    
   if (angle < 0)
     angle += 360; 
   else if (angle >= 360)
     angle -= 360; 

/*
    Serial.print("Angle: ");
    Serial.print(angle);    
    Serial.print("\trate: "); 
    Serial.println(rate);
*/    
    dispatch.gyroVal = angle;
  
}

//Instantiate the reflood maze with the most optimistic values
void instantiateReflood(){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcCenter(i, j, X);
    }
  }
}

void instantiate(){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcCenter(i, j, X);
      maze[j][i].walls = 15;
      //If this is the left column (0,x)
      if(i==0){
        maze[j][i].walls = 7;
      }
      //if this is the top row
      if(j==0){
        maze[j][i].walls = 14;
      }
      //if this is the bottom row
      if(j==(Y-1)){
        maze[j][i].walls = 13;
      }
      //If this is the righ column
      if(i==(X-1)){
        maze[j][i].walls = 11;
      }
      maze[0][0].walls = 6;
      maze[Y-1][0].walls = 5;
      maze[0][X-1].walls = 10;
      maze[X-1][Y-1].walls = 9;
    }
  }
}

void resetToCoord(coord desiredCoord){
  for(int j = 0; j<Y; j++){
    for(int i = 0; i<X; i++){
      maze[j][i].distance = calcDist(i, j, desiredCoord.x, desiredCoord.y);
    }
  }
}

//Get the most optimistic distance between two coordinates in a grid
int calcDist(byte posx, byte posy, byte desireX, byte desireY){
  int dist = (int) (abs(desireY-posy)+abs(desireX-posx));
  return dist;
}

//Get the most optimistic distance between a given coordinate and a 
//2x2 square in the center of a maze of dimension dim (dim must be even)
int calcCenter(byte posx, byte posy, byte dim){
  byte center = dim/2;
  int dist = 0;
  
  if(posy<center){
    if(posx<center){
      //You're in the top left of the maze
      dist=calcDist(posx, posy, (center-1), (center-1));
    }else{
      //You're in the top right of the maze
      dist=calcDist(posx,posy,center,(center-1));
    }
  }else{
    if(posx>=center){
      //You're in the bottom right of the maze
      dist=calcDist(posx,posy,center,center);
    }else{
      //You're in the bottom left of the maze
      dist=calcDist(posx,posy, (center-1),center);
    }
  }
return dist;
}

/*
INPUT: a coordinate representing a current position, and a heading
OUTPUT: the coordinates of the next desired position based on the heading and current position
*/
coord bearingCoord(coord currCoord, byte heading){
  coord nextCoord = {0,0};
  switch (heading){
    case 1:
      //code
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y-1;
      break;
    case 2:
      nextCoord.x=currCoord.x;
      nextCoord.y=currCoord.y+1;
      break;
    case 4:
      nextCoord.x=currCoord.x+1;
      nextCoord.y=currCoord.y;
      break;
    case 8:
      nextCoord.x=currCoord.x-1;
      nextCoord.y=currCoord.y;
      break;
  }
  return nextCoord;
}

/*
INPUT: A Coord representing the current coordiante and the robots current heading
OUTPUT: An optimal direction away from the current coordinate.
*/
byte orient(coord currCoord, byte heading){
  
  coord leastNext = {0,0};
  //This is the absolute largest value possible (dimension of maze squared)
  int leastNextVal = sizeof(maze)*sizeof(maze);
  byte leastDir = heading;
  
  //If there is a bitwise equivalence between the current heading and the cell's value, then the next cell is accessible
  if((maze[currCoord.x][currCoord.y].walls & heading) != 0){
    //Define a coordinate for the next cell based onthis heading and set the leastNextVal t its value
    coord leastnextTemp = bearingCoord(currCoord, heading);
    
    if(checkBounds(leastnextTemp)){
      leastNext = leastnextTemp;
      leastNextVal = maze[leastNext.y][leastNext.x].distance;
    }
  }
  
  for(int i=0; i<sizeof(headings); i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((maze[currCoord.y][currCoord.x].walls & dir) != 0){
      //define the coordiante for this dir
      coord dirCoord = bearingCoord(currCoord,dir);
      
      if(checkBounds(dirCoord)){
        //if this dir is more optimal than continuing straight
        if(maze[dirCoord.y][dirCoord.x].distance < leastNextVal){
          //update teh value of leastNextVal
          leastNextVal = maze[dirCoord.y][dirCoord.x].distance;
          //update the value of leastnext to this dir
          leastNext = dirCoord;
          leastDir = dir;
        }
      }
    }
  }
  return leastDir;
}

//Take a coordinate and test if it is within the allowable bounds
boolean checkBounds(coord Coord){
  if((Coord.x >= X) || (Coord.y >= Y) || (Coord.x < 0) || (Coord.y < 0)){return false;}else{return true;}
}

/*
INPUT: Coord
OUTPUT: An integer that is the least neighbor
*/
int checkNeighs(coord Coord){
  int minVal =  sizeof(maze)*sizeof(maze);
  for(int i=0; i<sizeof(headings); i++){
    byte dir = headings[i];
    //if this dir is accessible
    if((maze[Coord.y][Coord.x].walls & dir) != 0){
      //Get the coordinate of the accessible neighbor
      coord neighCoord = bearingCoord(Coord, dir);
      //Check the value of the accessible neighbor
      if (checkBounds(neighCoord)){
        //if the neighbore is less than the current recording minimum value, update the minimum value
        //If minVal is null, set it right away, otherwise test
        if(maze[neighCoord.y][neighCoord.x].distance < minVal){minVal = maze[neighCoord.y][neighCoord.x].distance;}
      }
    }
  }
  return minVal;
}

//Given a coordinate, test and return if the coordinate is bounded on three sides
boolean isDead(coord coord){
  boolean deadEnd = false;
  if(checkBounds(coord)){
    byte bounds = maze[coord.y][coord.x].walls;
    //bounds is the integer from the exploratory maze that represents the known walls of the coordinate
    if((bounds == 1)||(bounds == 2)||(bounds == 4) || (bounds == 8)){deadEnd=true;}
  }
  return deadEnd;
}

boolean isEnd(coord Coord, coord DesiredArray[]){
  boolean End = false;
  for(int i=0; i<sizeof(DesiredArray);i++){
    coord Desired = DesiredArray[i];
    if(checkBounds(Coord)){
      if((Coord.x == Desired.x)&&(Coord.y==Desired.y)){
        End = true;
      }
    }
  }
  return End;
}

/*
int readAhead(){
  return 0;
}
  */

/*
INPUT: Coordindate to update, and a direction representing the wall to add
OUTPUT: Update to coordinate adding the wall provided as an argument
*/

void coordUpdate(coord coordinate, byte wallDir){
  if(checkBounds(coordinate)){
    if((maze[coordinate.y][coordinate.x].walls & wallDir) != 0){
      maze[coordinate.y][coordinate.x].walls = maze[coordinate.y][coordinate.x].walls-wallDir;
    }
  }
}

/*
INPUT: Current Robot coordinate
OUTPUT: Update maze for learned walls
*/
void floodFillUpdate(coord currCoord, coord desired[]){
  StackList<coord> entries;
  
  maze[currCoord.y][currCoord.x].walls=readCurrent();
  entries.push(currCoord);
  
  for(int i=0; i<sizeof(headings); i++){
    byte dir = headings[i];
    //If there's a wall in this dir
    if((maze[currCoord.y][currCoord.x].walls & dir) == 0){
      //create a temporary working coordinate
      coord workingCoord = {currCoord.x,currCoord.y};
      switch(dir){
        case 1:
          workingCoord.y=workingCoord.y-1;
          coordUpdate(workingCoord,2);
          break;
        case 2:
          workingCoord.y=workingCoord.y+1;
          coordUpdate(workingCoord,1);
          break;
        case 4:
          workingCoord.x=workingCoord.x+1;
          coordUpdate(workingCoord, 8);
          break;
       case 8:
         workingCoord.x=workingCoord.x-1;
         coordUpdate(workingCoord,4);
         break;
      }
      //If the workingEntry is a valid entry and not a dead end, push it onto the stack
      if(checkBounds(workingCoord)&&(!isEnd(workingCoord, desired))){
        entries.push(workingCoord);
      }
    }
  }
  //While the entries stack isn't empty
  while(!entries.isEmpty()){
    //Pop an entry from the stack
    coord workingEntry = entries.pop();
    int neighCheck = checkNeighs(workingEntry);
    //If the least neighbor of the working entry is not one less than the value of the working entry
    if(neighCheck+1!=maze[workingEntry.y][workingEntry.x].distance){
      maze[workingEntry.y][workingEntry.x].distance=neighCheck+1;
      for(int i=0;i<sizeof(headings);i++){
        byte dir = headings[i];
        if((maze[workingEntry.y][workingEntry.x].walls & dir) != 0){
          coord nextCoord = bearingCoord(workingEntry,dir);
          if(checkBounds(nextCoord)){
            if(!isEnd(nextCoord, desired)){
              entries.push(nextCoord);
            }
          }
        }
      }
    }
  }
}



instruction createInstruction(coord currCoord, coord nextCoord, byte nextHeading){
  float change = 0.0;
  switch(nextHeading){
    case 1:
      if(globalHeading==4){
        change = -90.0;
      }
      if(globalHeading==8){
        change = 90.0;
      }
      if(globalHeading==2){
        change = 180.0;
      }
      break;
    case 2:
      if(globalHeading==4){
        change = 90.0;
      }
      if(globalHeading==8){
        change = -90.0;
      }
      if(globalHeading==1){
        change = 180.0;
      }
      break;
    case 4:
      if(globalHeading==1){
        change = 90.0;
      }
      if(globalHeading==2){
        change = -90.0;
      }
      if(globalHeading==8){
        change = 180.0;
      }
      break;
    case 8:
      if(globalHeading==1){
        change = -90.0;
      }
      if(globalHeading==2){
        change = 90.0;
      }
      if(globalHeading==4){
        change = 180.0;
      }
      break;
  }
  float desiredHeading = dispatch.gyroVal+change;
  //fix over or underflow
  
  if(((desiredHeading<45.0)||(desiredHeading>315.0))){
    desiredHeading=0.0;
  }
  if((desiredHeading>45.0)&&(desiredHeading<135.0)){
    desiredHeading = 90.0;
  }
  if((desiredHeading>135.0)&&(desiredHeading<225.0)){
    desiredHeading = 180.0;
  }
  if((desiredHeading>225.0)&&(desiredHeading<315.0)){
    desiredHeading = 270.0;
  }
  
  instruction turnMove = {7.74, desiredHeading};
  return turnMove;
}

void executeInstruction(instruction instruct){
  turn(instruct.desiredHeading);
  moveDist(instruct.desiredPos);
}

void floodFill(coord desired[], coord current, boolean isMoving){
  coord currCoord = current;
  byte heading = globalHeading;
  /*Integer representation of heading
  * 1 = N
  * 4 = E
  * 2 = S
  * 8 = W
  */
  while(maze[currCoord.y][currCoord.x].distance != 0){
    floodFillUpdate(currCoord, desired);
    byte nextHeading = orient(currCoord, heading);
    coord nextCoord = bearingCoord(currCoord, nextHeading);
      
    if(isMoving){
      //Call createInstruction to push a new instruction to the stack
      instructions.push(createInstruction(currCoord, nextCoord, nextHeading));
        
      //Pop the next instruction from the instructions queue and execute it
      executeInstruction(instructions.pop());
    }
      
    //After exectuing the instruction update the values of the local and global variables
    currCoord = nextCoord;
    heading = nextHeading;
    //If the robot has actually moved, update the global position variables
    if(isMoving){
      globalHeading = heading;
      globalCoord = currCoord;
    }
  }
  //Set the global end as the current coordinate.
  globalEnd = currCoord;
}

/*
INPUT: A heading
OUTPUT: A boolean indicating if the robot is oriented that way
*/

boolean isHeading(byte heading){
  boolean headingBool = false;
  switch(heading){
    //NORTH
    case 1:
      //if the robot's orientation is between 315 deg and 225 deg (pointing north)
      if((dispatch.gyroVal<315.0)&&(dispatch.gyroVal>225.0)){
        headingBool = true;
      }
      break;
    case 2:
      if((dispatch.gyroVal<135.0)&&(dispatch.gyroVal>45.0)){
        headingBool = true;
      }
      break;
    case 4:
      if((dispatch.gyroVal>315.0)&&(dispatch.gyroVal<45.0)){
        headingBool = true;
      }
      break;
    case 8:
      if((dispatch.gyroVal>135.0)&&(dispatch.gyroVal<225.0)){
        headingBool = true;
      }
      break;
  }
  return headingBool;
}

/*
INPUT: An error value for turning
OUTPUT: Put motore in forward/reverse for turning accoriding to error value
*/
void turnError(float error){
   //If the desiredPosition is greater than the currAngle then we need to rotate clockwise
   if(error>0.0){
     motorMove((error/90)*10.0, R_motor_backward,R_motor_forward);
     motorMove((error/90)*10.0, L_motor_forward,L_motor_backward);
   //if the currAngle is greater than the desired position we need to rotate counter clockwise
   }else if(error<0.0){
     motorMove((error/90)*10.0, R_motor_forward,R_motor_backward);
     motorMove((error/90)*10.0, L_motor_backward,L_motor_forward);
   }
}

void turn(float desiredPosition){
  
  if(desiredPosition==0.0){
    if(isHeading(1)){
      //this is ~270
      float currAngle = dispatch.gyroVal;
      //so make it ~-90
      currAngle-=360.0;
      //So the error will be ~90 because the desired position is greater than the current angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        if(dispatch.gyroVal>260.0){
          currAngle = dispatch.gyroVal-360.0;
        }
        error = (desiredPosition - currAngle);
      }
    }else if(isHeading(2)){
      //this is ~90
      float currAngle = dispatch.gyroVal;
      //So the error will be ~-90 because the desired position is less than the current angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        if(dispatch.gyroVal>260.0){
          currAngle = dispatch.gyroVal-360;
        }else{
          currAngle = dispatch.gyroVal;
        }
        error = (desiredPosition - currAngle);
      }
    }else if(isHeading(8)){
      //this is ~180
      float currAngle = dispatch.gyroVal;
      //So the error will be ~-180
      float error = (desiredPosition-currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        if(dispatch.gyroVal>350.0){
          currAngle = dispatch.gyroVal-360;
        }else{
          currAngle = dispatch.gyroVal;
        }
        error = (desiredPosition - currAngle);
      }
    }
  }else if(desiredPosition == 90.0){
    
    if(isHeading(1)||isHeading(8)){
      //this is ~270 or ~180
      float currAngle = dispatch.gyroVal;
      //We want the robot to rotate counter clockwise which means a negative error
      //So subtract the currangle from the desired angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        error = (desiredPosition - currAngle);
      }
    }else if(isHeading(4)){
      //this is ~0.0
      float currAngle = dispatch.gyroVal;
      //if it is over shot by a bit, subtrack 360
      if(currAngle<350.0){
        currAngle-=360.0;
      }
      //We want the robot to rotate clockwise which means a positive error
      //So subtract the currangle from the desired angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        error = (desiredPosition - currAngle);
      }
    }
  }else if(desiredPosition == 180.0){
    
    if(isHeading(1)){
      //this is ~270
      float currAngle = dispatch.gyroVal;
      //so we want to turn counter clockwise with a negative error value
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        error = (desiredPosition - currAngle);
      }
    }else if(isHeading(2)||isHeading(4)){
      
      //this is ~90 or ~0
      float currAngle = dispatch.gyroVal;
      //if it is over shot by a bit, subtrack 360
      if(currAngle<350.0){
        currAngle-=360.0;
      }
       //We want the robot to rotate clockwise which means a positive error
      //So subtract the currangle from the desired angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        error = (desiredPosition - currAngle);
      }
    }
  }else if(desiredPosition == 270.0){
    if(isHeading(4)){
      
      //this is ~0
      float currAngle = dispatch.gyroVal;
      //if it is undershot
      if(currAngle<5.0){
        currAngle+=360.0;
      }
      //We want the robot to rotate counter clockwise which means a negative error
      //So subtract the currangle from the desired angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        //if it is still undershot
        if(currAngle<5.0){
          currAngle+=360.0;
        }
        error = (desiredPosition - currAngle);
      }
    }else if(isHeading(2)||isHeading(8)){
      
      //this is ~90 or ~180
      float currAngle = dispatch.gyroVal;

      //We want the robot to rotate clockwise which means a positive error
      //So subtract the currangle from the desired angle
      float error = (desiredPosition - currAngle);
      while((error>1.0)||(error<-1.0)){
        turnError(error);
        gyroRead();
        currAngle = dispatch.gyroVal;
        error = (desiredPosition - currAngle);
      }
    }
  }
}

void moveDist(float dist){
  //diam is 2.32833 cm
  //rad = 1.2319
  //circ = 2*pi*r = 7.740
  
  //small = 12
  //big = 40
  float circ = 7.74;
  long encoder_count = (long)((dist/circ)*(40.0/12.0)*oneTurn);

  float motorSpeed = .1;
  
  long currCountRight = rightEnc.read();
  long currCountLeft = leftEnc.read();

  long desiredCount = currCountRight+encoder_count;
  
  float error=(desiredCount-abs(positionRight));
  
  int prevRightIR = dispatch.irValues[4];
  int prevLeftIR = dispatch.irValues[5];
  
  while(((error<-150)||(error>150))){
    
    float rightdamping = 1.0;
    float leftdamping = 1.0;
    
    float straigtDamp = 0.1;
    
    //Execute the stabalization only if the previously stored IR values was from a wall
    if(prevRightIR>100){
      //If the right IR sensor is reading higher than the previous wall reading
      if(dispatch.irValues[4]>prevRightIR){
        rightdamping+=straigtDamp;
        rightdamping-=straigtDamp;
      }
    }
    if(prevLeftIR>100){
      //If the left IR sensor is reading higher than the previous wall reading
      if(dispatch.irValues[5]>prevLeftIR){
        rightdamping-=straigtDamp;
        rightdamping+=straigtDamp;
      }
    }
    
    prevRightIR = dispatch.irValues[4];
    prevLeftIR = dispatch.irValues[5];
    
    error=(desiredCount-abs(positionRight));
    
    long newRight = rightEnc.read();
    positionRight = newRight;
    Serial.println(newRight);

    positionRight = newRight;
    currCountRight = newRight;
    /*
    Serial.println(desiredCount);
    Serial.println((damping*error/desiredCount)*10.0);
    Serial.println();
    */
    //if the desired count is greater than the position
    if(error>0){
      motorMove((rightdamping*error/desiredCount)*10.0, R_motor_forward,R_motor_backward);
      motorMove((leftdamping*error/desiredCount)*10.0, L_motor_forward,L_motor_backward);
    }else{
      motorMove((rightdamping*error/desiredCount)*10.0,R_motor_backward, R_motor_forward);
      motorMove((leftdamping*error/desiredCount)*10.0, L_motor_backward, L_motor_forward);
    }
  }

  motorMove(0.0, R_motor_forward,R_motor_backward);
  motorMove(0.0, L_motor_forward,L_motor_backward);
}

void motorMove(float perSpeed, int pin1, int pin2) {
  if(perSpeed==0.0){
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0); 
  }else if(perSpeed<3.0){perSpeed=3.0;}
   actSpeed = (perSpeed/100)*255;
   analogWrite(pin1, actSpeed);
   analogWrite(pin2, 0); 
}

void printMaze(){
  for(int j=0; j<Y; j++){
    for(int i=0; i<X; i++){
      Serial.print(maze[j][i].walls);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void reflood(){
  //Refill the maze for most optimistic values, but now the maze has walls
  instantiateReflood();
  
  //Run flood fill but without actual motion
  coord desired[] = {{X-1,Y-1},{X-1,Y},{X,Y-1},{X,Y}};
  coord currCoord = {0,0};
  floodFill(desired, currCoord, false);
  
  //Now, the robot is still at the start, but the maze distance values have been updated with the walls discovered
  //So we follow the maze creating instructions
  createSpeedQueue();
  //We now have a queue of instructions.
  
}

//Trace the maze back to the end creating instructions and adding them to the queue
void createSpeedQueue(){
  coord workingCoord = globalCoord;
  byte workingDir = globalHeading;
  int workingDist = 7.74;
  while((workingCoord.x!=globalEnd.x)&&(workingCoord.y!=globalEnd.y)){
    byte optimalDir = orient(workingCoord, workingDir);

    //If the direction is the same, accumulate distance
    if(optimalDir==workingDir){
      workingDist+=7.74;
    }else{
      //if the optimal is different from the working, add the working and the accumulated distance
      instruction nextInstruction = {workingDist, optimalDir};
      instructions.push(nextInstruction);
      //Reset the distance to one square and update the workingDir
      workingDist = 7.74;
      workingDir = optimalDir;
    }
    
    //update workingCoord to the next optimal coord
    workingCoord = bearingCoord(workingCoord, optimalDir);
  }
}

void loop(){
  
  Serial.println(dispatch.irValues[0]);
  Serial.println(dispatch.irValues[1]);
  Serial.println(dispatch.irValues[2]);
  Serial.println(dispatch.irValues[3]);
  Serial.println(dispatch.irValues[4]);
  Serial.println(dispatch.irValues[5]);
  Serial.println();
  
  
  
  
  coord desired[] = {{X-1,Y-1},{X-1,Y},{X,Y-1},{X,Y}};
  floodFill(desired, globalCoord, true);
  coord returnCoord[] = {{0,0}};
  resetToCoord(returnCoord[0]);
  //Run fill to return to the start coord
  floodFill(returnCoord, globalCoord, true);
  
  //Reflood the maze
  reflood();
  
  //Pop instructions from the front of the queue until its empty.
  while(!instructions.isEmpty()){
    executeInstruction(instructions.pop());
  } 
}
