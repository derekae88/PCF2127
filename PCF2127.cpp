/*
  PCF2127.cpp - Arduino library for communicating with the NXP PCF2127 Real Time Clock.
  Created by Derek Evans, June 5, 2016.
*/

#include "Arduino.h"
#include "PCF2127.h"

/********************************Public methods****************************/

/*****************************************************************************
 * 
 *	Function name : init
 *  
 *	Returns :		None
 *  
 *	Parameters :	int RTC_SS	->	The Slave Select or Chip Select pin used by 
 *									Arduino to select PCF2127AT.
 * 
 * 
 *	Purpose :	Initializes the PCF2127AT by setting the input slave select pin
 *				as OUTPUT and writing it HIGH.  Also initializes the SPI bus and
 *				sets the SPI bit order to MSBFIRST and the SPI data mode to 
 *				SPI_MODE3.
 * 					
 * 
 ******************************************************************************/
void PCF2127::init(int RTC_SS)
{
  pinMode(RTC_SS, OUTPUT);
  digitalWrite(RTC_SS, HIGH);
  _RTC_SS = RTC_SS;
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
}

/*****************************************************************************
 * 
 *	Function name : reset
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 * 
 *	Purpose :	Resets the PCF2127AT registers to its default factory settings.
 * 					
 * 
 ******************************************************************************/
void PCF2127::reset() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_1);
  SPI.transfer(B00001000);//00
  SPI.transfer(B00000000);//01
  SPI.transfer(B00000000);//02
  SPI.transfer(B10000000);//03
  SPI.transfer(B00000000);//04
  SPI.transfer(B00000000);//05
  SPI.transfer(B00000000);//06
  SPI.transfer(B00000000);//07
  SPI.transfer(B00000000);//08
  SPI.transfer(B00000000);//09
  SPI.transfer(B10000000);//0A
  SPI.transfer(B10000000);//0B
  SPI.transfer(B10000000);//0C
  SPI.transfer(B10000000);//0D
  SPI.transfer(B10000000);//0E
  SPI.transfer(B00000000);//0F
  SPI.transfer(B00000011);//10
  SPI.transfer(B00000000);//11
  SPI.transfer(B00000000);//12
  SPI.transfer(B00000000);//13
  SPI.transfer(B00000000);//14
  SPI.transfer(B00000000);//15
  SPI.transfer(B00000000);//16
  SPI.transfer(B00000000);//17
  SPI.transfer(B00000000);//18
  SPI.transfer(B00001000);//19
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : OTPRefresh
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	"It is recommended to process an one time programmable (OTP) refresh 
 *				once after the power is up and the oscillator is operating stable. 
 *				The OTP refresh takes less than 100 ms to complete.  To perform an 
 *				OTP refresh, bit OTPR has to be cleared (set to logic 0) and then 
 *				set to logic 1 again." - NXP PCF2127AT datasheet, page 13
 * 					
 ******************************************************************************/
void PCF2127::OTPRefresh() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CLOCKOUT_CONTROL_REGISTER);
  byte CCR = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CLOCKOUT_CONTROL_REGISTER);
  SPI.transfer(bitSet(CCR, 5));
  digitalWrite(_RTC_SS, HIGH);
  delay(100);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CLOCKOUT_CONTROL_REGISTER);
  CCR = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CLOCKOUT_CONTROL_REGISTER);
  SPI.transfer(bitClear(CCR, 5));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : lowPower
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Puts the PCF2127AT in its lowest power setting.
 * 					
 ******************************************************************************/
void PCF2127::lowPower() {
  //See page 66 of datasheet
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_3);
  byte CR3 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_TIMESTAMP_CONTROL_REGISTER);
  byte TSCR = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  bitSet(CR3, 7); //**********************************************************************************************************
  bitSet(CR3, 6); //Battery switch over function disabled, battery low detection disabled, extra power fail detection disabled
  bitSet(CR3, 5); //**********************************************************************************************************
  bitSet(TSCR, 6); //Timestamp function disabled
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CLOCKOUT_CONTROL_REGISTER);
  SPI.transfer(B00000111);//Lowest temperature measurement frequency and clock output high impedance
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_3);
  SPI.transfer(CR3);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_TIMESTAMP_CONTROL_REGISTER);
  SPI.transfer(TSCR);
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : readAllRegisters
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Prints binary values of all registers from the PCF2127AT to 
 *				the Serial Monitor.  NOTE: Serial.begin(baud rate) must be declared
 *				prior to calling this method!
 * 
 ******************************************************************************/
void PCF2127::readAllRegisters() {
  digitalWrite(_RTC_SS,LOW);
  SPI.transfer(READ_CONTROL_REGISTER_1);
  for (byte i=0x00;i<=0x19;i++) {
    byte result = SPI.transfer(0);
    Serial.print(i,HEX);
    Serial.print(" ");
    Serial.println(result,BIN);
   }
  digitalWrite(_RTC_SS,HIGH);
}


/*****************************************************************************
 * 
 *	Function name : setTime
 *  
 *	Returns :		None
 *  
 *	Parameters :	int year -> The year, for example 2016. Is converted to 
 *								range from 0 - 99.
 * 					byte month -> The month, 1-12
 *					byte day -> The day, 1-31
 * 					byte hour -> The hour, 0-23. Note: 24 hour mode only!
 *					byte minute -> The minute, 0-59
 *					byte second -> The second, 0-59
 * 
 *	Purpose :	Sets the PCF2127AT time with the input time parameters.
 * 					
 ******************************************************************************/
void PCF2127::setTime(int year, byte month, byte day, byte hour, byte minute, byte second) {
  digitalWrite(_RTC_SS,LOW);
  SPI.transfer(WRITE_TIME);
  SPI.transfer(byte2bcd(second));
  SPI.transfer(byte2bcd(minute));
  SPI.transfer(byte2bcd(hour));
  SPI.transfer(byte2bcd(day));
  SPI.transfer(byte2bcd(0)); //weekday, do not need it
  SPI.transfer(byte2bcd(month));
  SPI.transfer(byte2bcd(byte(year-2000)));
  digitalWrite(_RTC_SS,HIGH);
}

/*****************************************************************************
 * 
 *	Function name : readTime
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Reads the PCF2127AT time and stores it in second, hour, minute,
 *				day, month, and year public variables.
 * 
 ******************************************************************************/
void PCF2127::readTime() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_TIME);
  second = bcd2byte(SPI.transfer(0));
  minute = bcd2byte(SPI.transfer(0));
  hour = bcd2byte(SPI.transfer(0));
  day = bcd2byte(SPI.transfer(0));
  weekday = bcd2byte(SPI.transfer(0));
  month = bcd2byte(SPI.transfer(0));
  year = bcd2byte(SPI.transfer(0));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : enableAlarm
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Sets the appropriate bits in Control Register 2 to enable the 
 *				alarm function.
 * 					
 ******************************************************************************/
void PCF2127::enableAlarm() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitSet(CR2, 1));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : setAlarm
 *  
 *	Returns :		None
 *  
 *	Parameters :	byte alarmSecond -> the second, 0-59
 *					byte alarmMinute -> the minute, 0-59
 *					byte alarmHour -> the hour, 0-23. Note: 24 hour mode only!
 *					byte alarmDay -> the day, 1-31
 * 
 *	Purpose :	Sets the time when the alarm is triggered.  When an alarm is triggered,
 *				the interrupt pin is low.
 * 
 ******************************************************************************/
void PCF2127::setAlarm(byte alarmSecond, byte alarmMinute, byte alarmHour, byte alarmDay) {
  enableAlarm();
  alarmSecond = byte2bcd(alarmSecond);
  alarmMinute = byte2bcd(alarmMinute);
  alarmHour = byte2bcd(alarmHour);
  alarmDay = byte2bcd(alarmDay);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_ALARM);
  SPI.transfer(bitClear(alarmSecond, 7));
  SPI.transfer(bitClear(alarmMinute, 7));
  SPI.transfer(bitClear(alarmHour, 7));
  SPI.transfer(bitClear(alarmDay, 7));
  SPI.transfer(B10000000);
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : clearAF
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Clears the alarm flag in control regester 2 that is set when
 *				the alarm is triggered and sets the interrupt pin high.
 * 
 ******************************************************************************/
void PCF2127::clearAF() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitClear(CR2, 4));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : disableAlarm
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Disables the alarm function.
 * 
 ******************************************************************************/
void PCF2127::disableAlarm() {
  //This disables second, minute, hour, day, and weekday alarm enable bits by setting bit 7 to one.
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_ALARM);
  SPI.transfer(B10000000);
  SPI.transfer(B10000000);
  SPI.transfer(B10000000);
  SPI.transfer(B10000000);
  SPI.transfer(B10000000);
  digitalWrite(_RTC_SS, HIGH);
  //This clears the alarm interrupt enable bit in control register 2, disabling interrupt generation by alarms.
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitClear(CR2, 1));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : enableCountdownTimer
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Sets the appropriate bits in Control Register 2 to enable the 
 *				countdown timer function.
 * 
 ******************************************************************************/
void PCF2127::enableCountdownTimer() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitSet(CR2, 0));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : setCountdownTimer
 *  
 *	Returns :		None
 *  
 *	Parameters :	int period -> The period of the countdown timer.  See the 
 *									datasheet for more details.
 * 
 *	Purpose :	Enables the countdown timer function by setting bit 0 in the co
 * 
 ******************************************************************************/
void PCF2127::setCountdownTimer(int period) {
  enableCountdownTimer();
  if (period <= 60) {
    byte n = period * 1; //countdown period = n/source clock frequency;
    digitalWrite(_RTC_SS, LOW);
    SPI.transfer(WRITE_WD_TIMER_CONTROL_REGISTER);
    SPI.transfer(B01000010);  //enable countdown timer, set interrupt signal to permanently active, and the timer source clock to 1 Hz
    SPI.transfer(n); //n is the watchdog timer value
    digitalWrite(_RTC_SS, HIGH);
  } else if (period > 60) {
    byte n = period / 60;
    digitalWrite(_RTC_SS, LOW);
    SPI.transfer(WRITE_WD_TIMER_CONTROL_REGISTER);
    SPI.transfer(B01000011);
    SPI.transfer(n);
    digitalWrite(_RTC_SS, HIGH);
  }
}

/*****************************************************************************
 * 
 *	Function name : clearCDTF
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Clears the alarm flag in control regester 2 that is set when
 *				the alarm is triggered and sets the interrupt pin high.
 * 
 ******************************************************************************/
void PCF2127::clearCDTF() {
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitClear(CR2, 3));
  digitalWrite(_RTC_SS, HIGH);
}

/*****************************************************************************
 * 
 *	Function name : disableCountdownTimer
 *  
 *	Returns :		None
 *  
 *	Parameters :	None
 * 
 *	Purpose :	Disables the countdown timer function.
 * 					
 ******************************************************************************/
void PCF2127::disableCountdownTimer() {
  //This disables the countdown timer bits (7 and 6) in the watchdog timer control register
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_WD_TIMER_CONTROL_REGISTER);
  SPI.transfer(B00000011);
  SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  //This sets the countdown timer interrupt enable bit to 0 in control register 2
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(READ_CONTROL_REGISTER_2);
  byte CR2 = SPI.transfer(0);
  digitalWrite(_RTC_SS, HIGH);
  digitalWrite(_RTC_SS, LOW);
  SPI.transfer(WRITE_CONTROL_REGISTER_2);
  SPI.transfer(bitClear(CR2, 0));
  digitalWrite(_RTC_SS, HIGH);
}

/********************************Private methods****************************/

/*****************************************************************************
 * 
 *	Function name : byte2bcd
 *  
 *	Returns :		byte
 *  
 *	Parameters :	byte num -> The value to convert to binary coded decimal.
 * 
 *	Purpose :	Converts a byte value to binary coded decimal.
 * 
 ******************************************************************************/
byte PCF2127::byte2bcd (byte num) {
  return (num/10) * 16 + num % 10;
}

/*****************************************************************************
 * 
 *	Function name : bcd2byte
 *  
 *	Returns :   byte
 *  
 *	Parameters :	byte num -> The value to convert to a byte.
 * 
 *	Purpose :	Converts a binary coded decimal value to byte.
 * 
 ******************************************************************************/
byte PCF2127::bcd2byte(byte num) {
  return ((num/16 * 10) + (num % 16));
}
