
int readSensor(int recPin)
{
  int returnVal = analogRead(recPin);
  digitalWrite(ledPin,LOW);
  return returnVal;
}

void powerUp(int ledPin){
    digitalWrite(ledPin, HIGH);
}

void loop(){
}
