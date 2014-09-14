/*

//storage variables
long timer=0;// time between one full rotation (in ms)


void setup(){

 
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 3000;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  //END TIMER SETUP
  
  Serial.begin(115200);
  
  attachInterrupt(0, displayMPH, CHANGE);
}


//ISR(TIMER1_COMPA_vect) {//Interrupt at freq of 1kHz to measure reed switch
 //timer+=1; 
//}

void displayMPH(){
 // if(millis()<=1000){
    timer+=1;
   Serial.println(timer); 
  //}
  
}

void loop(){
  //print mph once a second
  //if(millis()<=1000){
    //displayMPH();
  //}
}
*/


long timer;

void setup()
{
//  pinMode(pin, OUTPUT);
  attachInterrupt(0, blink, CHANGE);
  Serial.begin(115200);
}

void loop()
{
 
}

void blink()
{
  timer+=1;
  Serial.println(timer);
}
