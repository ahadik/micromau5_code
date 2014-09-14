/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2,6);
//   avoid using pins with LEDs attached



void setup() {
  Serial.begin(9600);
  //pinMode(5, OUTPUT);
  //digitalWrite(5, LOW);
  //pinMode(6, OUTPUT);
  //digitalWrite(6, LOW);
  Serial.println("Basic Encoder Test:");
}

long oldPosition  = 0;

void loop() {
  //long newPosition = myEnc.read();
  Serial.println(myEnc.read());
  /*if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }*/
//  char pin5 = digitalRead(5);
//  Serial.println(pin5);
}
