/*
  PCF2127.h - Arduino library for communicating with the NXP PCF2127 Real Time Clock.
  Created by Derek Evans, June 5, 2016.
*/

#ifndef PCF2127_h
#define PCF2127_h

//Write Addresses (001xxxxx);
#define WRITE_TIME B00100011
#define WRITE_ALARM B00101010
#define WRITE_CONTROL_REGISTER_1 B00100000
#define WRITE_CONTROL_REGISTER_2 B00100001
#define WRITE_WD_TIMER_CONTROL_REGISTER B00110000
#define WRITE_CLOCKOUT_CONTROL_REGISTER B00101111
#define WRITE_CONTROL_REGISTER_3 B00100010
#define WRITE_TIMESTAMP_CONTROL_REGISTER B00110010

//Read Addresses (101xxxxx);
#define READ_TIME B10100011
#define READ_ALARM B10101010
#define READ_CONTROL_REGISTER_1 B10100000
#define READ_CONTROL_REGISTER_2 B10100001
#define READ_CONTROL_REGISTER_3 B10100010
#define READ_WD_TIMER_CONTROL_REGISTER B10110000
#define READ_CONTROL_REGISTER_3 B10100010
#define READ_TIMESTAMP_CONTROL_REGISTER B10110010
#define READ_CLOCKOUT_CONTROL_REGISTER B10101111

#include "Arduino.h"
#include "SPI.h"

class PCF2127
{
  public:
	void init(int RTC_SS);
    void reset();
    void OTPRefresh();
    void lowPower();
    void readAllRegisters();
    void setTime(int year, byte month, byte day, byte hour, byte minute, byte second);
    void readTime();
    void enableAlarm();
    void setAlarm(byte alarmSecond, byte alarmMinute, byte alarmHour, byte alarmDay);
    //void readAlarm();
    void clearAF();
    void disableAlarm();
    void enableCountdownTimer();
    void setCountdownTimer(int period);
    void clearCDTF();
    void disableCountdownTimer();
    byte second;
	byte minute;
	byte hour;
	byte day;
	byte weekday;
	byte month;
	byte year;
  private:
  	byte bcd2byte(byte num);
    byte byte2bcd(byte num);
    int _RTC_SS;
};

#endif