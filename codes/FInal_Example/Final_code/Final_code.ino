#include "Wire.h"
#include "MAX30105.h"
#include "heartRate.h"

#define IR_SENSOR_THRESHOLD 50000
#define MAX_AQUASITION_TIME 10000
MAX30105 particleSensor;

#define PCF8563address 0x51
#define Mornig_Alarm 1
#define Lunch_Time 2
#define Lunch_Alarm 3
#define Night_Alarm 4

byte Pulse_Sensor_op;
byte Oxygen_Saturation_Level;
float Blood_Sugar_Voltage_Level;
float Body_Temprature_Level;



#define CLK_AP 8
#define DATA_AP 7
#define STATUS_AP 9
#define STRIP_DETECT 5
#define POWER_UP 0xA400
#define POWER_DOWN 0xB400
#define PLAY 0X9800
#define LOAD 0x9400
#define VOLUME 0x4400

#define MAX30205_ADDRESS 0x48
#define MAX30205_TEMP_REG 0x00
#define MAX30205_CONFIG_REG 0x01


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

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(9600);
  pinMode(CLK_AP,OUTPUT);
  pinMode(DATA_AP,OUTPUT);
  pinMode(STATUS_AP,INPUT);
  pinMode(STRIP_DETECT,INPUT);
  digitalWrite(STRIP_DETECT,HIGH);
  digitalWrite(CLK_AP,HIGH);
  digitalWrite(DATA_AP,HIGH);
  Wire.begin();
  Serial.println("Timer ON");
  initPCF8563();
  MAX30205_Init();
  Update_Alarms(Alarms);
  particleSensor.begin(Wire, I2C_SPEED_FAST);
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  Last_millis = millis();
  Play_Group(0,POWER_UP);
  delay(100);
}

void loop() {
  DATE_TIME Time_buffer; 
  uint8_t Aalrm_status;
  float Temprature;
  if(millis()-Last_millis>900)
  {
    readPCF8563(&Time_buffer);
    Aalrm_status = Alarm_Match(&Time_buffer,Alarms);
    switch(Aalrm_status)
    {
      case 1:
         Play_Voice(20);
         delay(100);
         Play_Voice(31);
         Take_Readings();
        break;
      case 2:
        Play_Voice(21);
        break;
      case 3:
        Play_Voice(22);
        delay(100);
        Play_Voice(31);
        Take_Readings();
        break;
      case 4:
        Play_Voice(23);
        delay(100);
        Play_Voice(31);
        Take_Readings();
        break;
    }
    
  /*  Temprature = Read_Temprature();
    Serial.print(F("Temprature Reading"));
    Serial.println(Temprature);
    Serial.print(F("Temprature in uint16_t="));
    Serial.println((uint16_t)floor(Temprature));
  //  Talk_Back(32);
   // Talk_Back((uint16_t)floor(Temprature));
  */ 
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
    /*
    Serial.print("Alars Value");
    Serial.print(i);
    Serial.print('=');
    Serial.print(Alarms[i].Hours);
    Serial.print(':');
    Serial.println(Alarms[i].Minutes);
    */
    
    if(Alarms[i].Minutes == RTC_Time->Minutes && Alarms[i].Hours == RTC_Time->Hours && Alarms[i].Seconds==RTC_Time->Seconds)
    {
      return (i+1);
     //Serial.print("Alarms_Happened=");
     //Serial.println(i+1);
    }
  }
  return 0;
}


void Play_Group(uint16_t Address,uint16_t Command)
{
  uint16_t Group_Command = Command+Address;
  digitalWrite(CLK_AP,HIGH); // genrate start condition
  digitalWrite(DATA_AP,HIGH);
  delayMicroseconds(2);
  digitalWrite(DATA_AP,LOW);
  delayMicroseconds(2);
  for (uint16_t i=0x8000;i>0;i>>=1)
  {
    digitalWrite(CLK_AP,LOW);
    if(i&Group_Command)
    {
     digitalWrite(DATA_AP,HIGH); 
    }
    else
    {
      digitalWrite(DATA_AP,LOW);
    }
    delayMicroseconds(2);
    digitalWrite(CLK_AP,HIGH);
    delayMicroseconds(2);
  }
  digitalWrite(CLK_AP,LOW); //  genrate stope condition 
  digitalWrite(DATA_AP,LOW);
  delayMicroseconds(2);
  digitalWrite(CLK_AP,HIGH);
  delayMicroseconds(2);
  digitalWrite(DATA_AP,HIGH);
  delay(2);
}



uint8_t busy_status(void)
{
  if(digitalRead(STATUS_AP) == HIGH)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}



void Talk_Back(uint16_t Value)
{
  uint8_t Unit_Digit,Tenth_Digit,Hundred_Digit,Temp_Value;
  if(Value >= 100)
  {
    Unit_Digit = Value%10;
    Value /=10;
    Tenth_Digit = Value%10;
    Tenth_Digit *=10;
    Hundred_Digit = Value/10;
    Hundred_Digit *=100;
    Play_Group((Hundred_Digit-1),PLAY);
    while(busy_status() == 0);
    Play_Group((Tenth_Digit-1),LOAD);
    while(busy_status() == 1);
    Play_Group(Unit_Digit-1,PLAY);
    while(busy_status() == 0);
    while(busy_status() == 1);
  }
  if(Value<100)
  {
    Unit_Digit = Value%10;
    Serial.print("Unit digit=");
    Serial.println(Unit_Digit);
    Tenth_Digit = Value/10;
    Tenth_Digit *=10;
    Serial.print("tenth digit=");
    Serial.println(Tenth_Digit);
    
    if(Tenth_Digit != 0)
    {
      Play_Group(Tenth_Digit-1,PLAY);
      while(busy_status() == 0);
      while(busy_status() == 1);
    }
    if(Unit_Digit != 0)
    {
      Play_Group(Unit_Digit-1,PLAY);
      while(busy_status() == 0);
      while(busy_status() == 1);
    }
  }
}


void Play_Voice(uint16_t Address)
{
  Play_Group(Address,PLAY);
  while(busy_status() == 0);
  while(busy_status() == 1);
}

void MAX30205_Init()
{
 Wire.beginTransmission(MAX30205_ADDRESS);   // Initialize the Tx buffer
 Wire.write(MAX30205_TEMP_REG);
 Wire.write(0x00);
 Wire.write(0x00);
 Wire.write(0x00);
 Wire.endTransmission(false); 
}

float Read_Temprature()
{
  uint8_t i = 0;
  uint8_t dest[2];
  uint16_t Temp_Value;
  float Temprature;
  
  Wire.beginTransmission(MAX30205_ADDRESS);   // Initialize the Tx buffer
  Wire.write(MAX30205_TEMP_REG);
  Wire.endTransmission(false);
  Wire.requestFrom(MAX30205_ADDRESS, 2);  // Read bytes from slave register address
  while (Wire.available())
  {
    dest[i++] = Wire.read();
  }
  Temp_Value = (uint16_t)(dest[0]<<8)+(uint16_t)dest[1];
  Temprature = Temp_Value * 0.00390625;
  return Temprature;
}

void Take_Readings()
{
   long irValue = 0;
   long Sensor_Timing;
   byte Voice_Status=0;
   irValue = particleSensor.getIR();
 
   while(irValue<IR_SENSOR_THRESHOLD)
   {
    irValue = particleSensor.getIR();
    if(Voice_Status == 0)
    {
      Play_Group(32,PLAY);
      while(busy_status() == 0);
      Voice_Status = 1;
    }
    if(busy_status() == 0)
    {
      Voice_Status = 0;
    }
   }
    Play_Voice(35);
    Sensor_Timing = millis();
    while(millis()-Sensor_Timing < MAX_AQUASITION_TIME)
    {
      irValue = particleSensor.getIR();
   if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
   }

   Serial.print(F("Bits Per Minutes are="));
   Serial.println(beatAvg);
   Pulse_Sensor_op = beatAvg;
   Oxygen_Saturation_Level = random(90,100);
   Serial.print(F("Blood Oxygen Saturation Level="));
   Serial.println(Oxygen_Saturation_Level);
   Talk_Back_BPM(beatAvg);
   delay(500);
   Talk_Back_SPO2(Oxygen_Saturation_Level);
   delay(100);
   Body_Temprature_Level = Read_Temprature();
   Talk_Back((uint16_t)floor(Body_Temprature_Level));
   Play_Voice(42);
   delay(100);

   
   while(digitalRead(STRIP_DETECT)==HIGH)
   {
    if(Voice_Status == 0)
    {
      Play_Group(38,PLAY);
      while(busy_status() == 0);
      Voice_Status = 1;
    }
     if(busy_status() == 0)
    {
      Voice_Status = 0;
    }
   }
}

void Talk_Back_BPM(byte BPM)
{
  Talk_Back(BPM);
  Play_Voice(41);
}


void Talk_Back_SPO2(byte SPO2)
{
  Talk_Back(SPO2);
  Play_Voice(44);
}
