#include "Wire.h"
#define PCF8563address 0x51

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
String days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

byte bcdToDec(byte value){
  return ((value / 16) * 10 + value % 16);
}
 
byte decToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}

void setPCF8563(){
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.write(decToBcd(second));  
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));     
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(dayOfWeek));  
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void readPCF8563(){
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address, 7);
  second     = bcdToDec(Wire.read() & B01111111); 
  minute     = bcdToDec(Wire.read() & B01111111); 
  hour       = bcdToDec(Wire.read() & B00111111); 
  dayOfMonth = bcdToDec(Wire.read() & B00111111);
  dayOfWeek  = bcdToDec(Wire.read() & B00000111);  
  month      = bcdToDec(Wire.read() & B00011111);  
  year       = bcdToDec(Wire.read());
}


rtc convertion setting timing and date
if( Serial.available())
{
char ch = Serial.read();
if( isDigit(ch) )// is this an ascii digit between 0 and 9?
{
value = (value * 10) + (ch - '0'); // yes, accumulate the value
}
else if (ch == 10) // is the character the newline character?
{
blinkRate = value; // set blinkrate to the accumulated value
Serial.println(blinkRate);
value = 0; // reset val to 0 ready for the next sequence of digits
}
}
