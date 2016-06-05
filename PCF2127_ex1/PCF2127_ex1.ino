/*This sketch shows the basics of using the PCF2127 library.*/

#include <SPI.h> //You must include the SPI library along with the PCF2127 library!
#include <PCF2127.h>

PCF2127 rtc;

void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT_PULLUP); //For generating an interrupt on arduino pin 2 with PCF2127 interrupt
  setInterrupt(); 
  rtc.init(10); //initialize the pcf2127
  rtc.reset(); //reset the pcf2127 to factory settings
  rtc.OTPRefresh(); //suggested to do a one time programable reset on power up
  delay(100); //OPTRefresh takes less than 100 ms
  rtc.setTime(2016,6,3,12,0,0); //set the rtc time to June 3, 2016 at 12:00:00 (noon)
  rtc.enableAlarm(); //enable the alarm on the PCF2127
  rtc.setAlarm(10,0,12,3); //Set the alarm for third day of the month on 12:00:10 (e.g. in ten seconds).  Will write the INT pin low when alarm is triggered.
}

void loop() {
  delay(1000);
  getTime(); //read time from the PCF2127 and pring to serial monitor
  if(rtc.second==15) {
    rtc.disableAlarm(); //disable the alarm
    rtc.clearAF(); //clear the interrupt.  This will bring the INT pin on the PCF2127 from LOW to HIGH (the INT pin is active low)
  }
}

void getTime() {
  rtc.readTime(); //Read the PCF time and get the values from the class variables month, day, year, hour, minute, second
  Serial.print(rtc.month);
  Serial.print("/");
  Serial.print(rtc.day);
  Serial.print("/");
  Serial.print(rtc.year);
  Serial.print(" ");
  Serial.print(rtc.hour);
  Serial.print(":");
  Serial.print(rtc.minute);
  Serial.print(":");
  Serial.println(rtc.second); 
}

//Set an interrupt on pin 2 (int 0) on arduino.  This will call the rtcISR function.
void setInterrupt() {
  noInterrupts();
  attachInterrupt(0, rtcISR, LOW); 
  interrupts ();
}

//Print message to serial monitor when interrupt is triggered by alarm
void rtcISR() {
  detachInterrupt(0);
  Serial.println("An interrupt was triggered!");
}


