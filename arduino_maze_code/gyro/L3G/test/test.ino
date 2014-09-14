#include <Wire.h> 

#include <L3G.h> 

L3G gyro;


int sampleNum=500; 

long dc_offset=0; 
double noise=0; 
unsigned long time; 
int sampleTime=10; 
int rate; 
int prev_rate=0; 
double angle=0;

void setup(){
  Serial.begin(115200); 
  Wire.begin(); 
  while (!gyro.init()); 
    gyro.enableDefault(); 

  //Calculate initial DC offset and noise level of gyro 
  for(int n=0;n<sampleNum;n++){ 
    gyro.read();
//    Serial.println((int)gyro.g.z);
    dc_offset+=(long)gyro.g.z; 
    //Serial.println(dc_offset);
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
  
  //print dc offset and noise level 

  Serial.println(); 
  Serial.print("DC Offset: "); 
  Serial.print(dc_offset); 
  Serial.print("\tNoise Level: "); 
  Serial.print(noise); 
  Serial.println(); 
  delay(7000); 

}

void loop(){
  
  // Every 10 ms take a sample from the gyro 
  if(millis() - time > sampleTime){ 
    time = millis(); // update the time to get the next sample 
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

    
    //Serial.println(dc_offset);


    Serial.print("angle: "); 
    Serial.print(angle); 
    Serial.print("\trate: "); 
    Serial.println(rate); 

  } 
}
