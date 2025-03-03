#include "Wire.h"
#define PCF8563address 0x51
#define Mornig_Alarm 1
#define Lunch_Time 2
#define Lunch_Alarm 3
#define Night_Alarm 4



typedef struct Date_Time{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
  uint8_t DayOfMonth;
  uint8_t DayOfWeek;
  uint8_t Month;
  uint8_t Year;
}DATE_TIME;



typedef struct Alarm_Time{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
}ALARM_TIME;

ALARM_TIME Alarms[4];
char index=0;
char buffer_serial[10];
long Last_millis;
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Timer ON");
  initPCF8563();
  Update_Alarms(Alarms);
  Last_millis = millis();
  // put your setup code here, to run once:
}

void loop() {
  DATE_TIME Time_buffer; 
  uint8_t Aalrm_status;
  
  if(millis()-Last_millis>900)
  {
    readPCF8563(&Time_buffer);
    Aalrm_status = Alarm_Match(&Time_buffer,Alarms);
    Serial.println(Aalrm_status);
    Print_Date_Time(&Time_buffer);
    Last_millis=millis();
  }
  
  if(Serial.available() > 6)
  {
    get_Time_Date(&Time_buffer);
  }
  // put your main code here, to run repeatedly:
}

byte bcdToDec(byte value){
  return ((value / 16) * 10 + value % 16);
}


byte decToBcd(byte value){
  return (value / 10 * 16 + value % 10);
}


void setPCF8563_Date(DATE_TIME* Time_Buffer){
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x05);
//Wire.write(decToBcd(Time_Buffer->Second));  
//Wire.write(decToBcd(Time_Buffer->Minute));
//Wire.write(decToBcd(Time_Buffer->Hour));     
  Wire.write(decToBcd(Time_Buffer->DayOfMonth));
  Wire.write(decToBcd(Time_Buffer->DayOfWeek));  
  Wire.write(decToBcd(Time_Buffer->Month));
  Wire.write(decToBcd(Time_Buffer->Year));
  Wire.endTransmission();
}



void setPCF8563_Time(DATE_TIME* Time_Buffer){
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.write(decToBcd(Time_Buffer->Seconds));  
  Wire.write(decToBcd(Time_Buffer->Minutes));
  Wire.write(decToBcd(Time_Buffer->Hours));     
//Wire.write(decToBcd(Time_Buffer->DayOfMonth));
//Wire.write(decToBcd(Time_Buffer->DayOfWeek));  
//Wire.write(decToBcd(Time_Buffer->Month));
//Wire.write(decToBcd(Time_Buffer->Year));
  Wire.endTransmission();
}


void readPCF8563(DATE_TIME* Time_Buffer){
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(PCF8563address,7);
  Time_Buffer->Seconds = bcdToDec(Wire.read() & B01111111); 
  Time_Buffer->Minutes = bcdToDec(Wire.read() & B01111111); 
  Time_Buffer->Hours = bcdToDec(Wire.read() & B00111111); 
  Time_Buffer->DayOfMonth = bcdToDec(Wire.read() & B00111111);
  Time_Buffer->DayOfWeek = bcdToDec(Wire.read() & B00000111);  
  Time_Buffer->Month = bcdToDec(Wire.read() & B00011111);  
  Time_Buffer->Year = bcdToDec(Wire.read());
}

void initPCF8563()
{
  uint8_t Temp;
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();
  /*Wire.requestFrom(PCF8563address, 1);
  Temp = Wire.read();
  Temp&=~(0x01<<5);
  Wire.beginTransmission(PCF8563address);
  Wire.write(0x00);
  Wire.write(Temp);
  */
}
void Print_Date_Time(DATE_TIME* Time_Buffer)
{
  Serial.print("Time:-");
  Serial.print(Time_Buffer->Hours);
  Serial.print(':');
  Serial.print(Time_Buffer->Minutes);
  Serial.print(':');
  Serial.println(Time_Buffer->Seconds);
  Serial.print("Date:-");
  Serial.print(Time_Buffer->DayOfMonth);
  Serial.print('/');
  Serial.print(Time_Buffer->Month);
  Serial.print('/');
  Serial.println(Time_Buffer->Year + 2000);
}



/*
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
*/
void Update_Alarms(ALARM_TIME* Alarms)
{
  Alarms[0].Hours = 9; // setting the mornig cehck-up routine time
  Alarms[0].Minutes = 00; //
  Alarms[0].Seconds = 00;

  Alarms[1].Hours = 13; // setting the lunch time
  Alarms[1].Minutes = 00; //
  Alarms[1].Seconds = 00;

  Alarms[2].Hours = 14; // setting the after lunch cehck-up routine time
  Alarms[2].Minutes = 00; //
  Alarms[2].Seconds = 00;

  Alarms[3].Hours = 21; // setting the night cehck-up routine time
  Alarms[3].Minutes = 00; //
  Alarms[3].Seconds = 00;
}



void get_Time_Date(DATE_TIME* time_date)
{
  byte Temp_Data;
  char Low_digit = 0,High_digit = 0;
  uint8_t Complete_Digit;
  Temp_Data = Serial.read();
  Temp_Data -='0';
  if(Temp_Data == 1)
  {
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->Hours = ((High_digit*10)+Low_digit);
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->Minutes = (High_digit*10)+Low_digit;
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->Seconds= (High_digit*10)+Low_digit;
    setPCF8563_Time(time_date);
    Serial.flush();
    Serial.println("time has been set");
  }
  else
  {
    while(Serial.available()<8);
    //wait for Date entry  setup
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->DayOfMonth = (High_digit*10)+Low_digit;
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->DayOfWeek = (High_digit*10)+Low_digit;
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->Month= (High_digit*10)+Low_digit;
    High_digit = Serial.read()-'0';
    Low_digit = Serial.read()-'0';
    time_date->Year= (High_digit*10)+Low_digit;
    setPCF8563_Date(time_date);
    Serial.flush();
    Serial.println("date has been set");
  }
}



uint8_t Alarm_Match(DATE_TIME* RTC_Time,ALARM_TIME* Alarms)
{
  for(uint8_t i=0;i<4;i++)
  {
    Serial.print("Alars Value");
    Serial.print(i);
    Serial.print('=');
    Serial.print(Alarms[i].Hours);
    Serial.print(':');
    Serial.println(Alarms[i].Minutes);
    
    if(Alarms[i].Minutes == RTC_Time->Minutes && Alarms[i].Hours == RTC_Time->Hours && Alarms[i].Seconds==RTC_Time->Seconds)
    {
      return (i+1);
     //Serial.print("Alarms_Happened=");
     //Serial.println(i+1);
    }
  }
  return 0;
}
