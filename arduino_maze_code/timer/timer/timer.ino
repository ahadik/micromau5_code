#include <Eventmicro.h>
#include <Timermicro.h>

#include <Event.h>
#include <Timer.h>

Timer tm;
Timermicro t;

int countm=0;
int count=0;

void setup(){
  Serial.begin(115200);
    t.every(1, countup);
  tm.every(1,countupm);
}

void countupm(){
  if(millis()<3000){
    countm+=1;
    Serial.println("m"+String(countm));
  }
}

void countup(){
  if(millis()<3000){
    count+=1;
    Serial.println(count);
  }
}

void loop(){
  t.update();
  tm.update();
}
