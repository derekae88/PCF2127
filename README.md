# PCF2127
Arduino library for the NXP PCF2127 Real Time Clock.

## Connections for PCF2127AT (SO20 Package) to Arduno Uno or Atmega328

pin 1 -> Arduino Uno pin 13 or Atmega328 pin 19 (Clock)

pin 2 -> Arduino Uno pin 11 or Atmega328 pin 17 (Master Out Slave In)

pin 3 -> Arduino Uno pin 12 or Atmega328 pin 18 (Master In Slave Out)

pin 4 -> Arduino Uno pin 10 or Atmega328 pin 16 (Chip/Slave Select)

pin 5 -> GND (This selects the SPI bus)

pin 8 -> GND (Vss)

pin 20 -> 3.3V (Vdd)

pin 17 (RTC Interrupt Generation) -> Arduino pin 2 or 3 or Atmega pin 4 or 5 (Interrupt 0/1)

NOTE: The PCF2127 requires 1.8V to 4.2V for operation.  The Arduino Uno operates at 5V so I had converted an Arduino Uno from 5V to 3.3V to utilize with the PCF2127.  I used the technique described at https://learn.adafruit.com/arduino-tips-tricks-and-techniques/3-3v-conversion to convert an Arduino Uno to 3.3V.

