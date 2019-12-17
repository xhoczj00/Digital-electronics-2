
# GPS module project

#### Table of Contents

1. [Introduction](#introduction)
2. [Hardware](#hardware)
3. [Software](#software)
4. [Commit tags](#Commit_tags)
5. [References](#references)


## Introduction

Result of this project should be working UART communication between GPS module and microcontroller ATmega328p. 
Recieved data should be displayed on Nokia 5110 display and stored in PC.


## Hardware

- Microcontroller - ATmega2560
- GPS Module - PA6H
- Display - LCD 5110 (PCD 8544)

## Software

- Putty

## Commit_tags

- GENERAL - Commits with general changes 
- GPS - Commits with changes in GPS code
- DISPLAY - Commits with changes in DISPLAY code
- COMMUNICATION - Commits with changes in communication code

## References

1. Microchip, [AVR 8-bit Toolchain for Windows, Linux, Mac OS X](https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers)
2. Adafruit, [Adafruit Ultimate GPS](https://airu.coe.utah.edu/wp-content/uploads/sites/62/2017/09/adafruit-ultimate-gps.pdf?fbclid=IwAR0Ea351FrcKBUMDiNXTcYiiYndw2nQmZJZgRo1zAhL7gRVmd4eEwBCMVXM)
3. DENISOV Sergey, [Nokia 5110 C library](https://github.com/LittleBuster/avr-nokia5110)
4. GOGO:Tronics, [Nokia5110 LCD Image Creator](https://sparks.gogo.co.nz/pcd8554-bmp.html)
5. BADDELEY Glenn, [GPS - NMEA sentence information](http://aprs.gids.nl/nmea/?fbclid=IwAR0fBH6iihk2jP2ggj2fvoQDHA2099OemoPaujcPFWmtm5nfdvbKm0RUfD8#gsa)
6. GPSinformation, [NMEA data](https://www.gpsinformation.org/dale/nmea.htm?fbclid=IwAR3vzXEpJMyglt1sn1XzM4cIh-r2uJDDzEd3iujcXd3tYgysBgcucY19lPU#GGA)
