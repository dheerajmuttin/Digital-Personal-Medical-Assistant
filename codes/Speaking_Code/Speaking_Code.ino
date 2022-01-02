#define CLK_AP 8
#define DATA_AP 7
#define STATUS_AP 9
#define POWER_UP 0xA400
#define POWER_DOWN 0xB400
#define PLAY 0X9800
#define LOAD 0x9400
#define VOLUME 0x4400



void setup() {
  pinMode(CLK_AP,OUTPUT);
  pinMode(DATA_AP,OUTPUT);
  pinMode(STATUS_AP,INPUT);
  digitalWrite(CLK_AP,HIGH);
  digitalWrite(DATA_AP,HIGH);
  Serial.begin(9600);
  // put your setup code here, to run once:
}



void loop() {
  Play_Group(0,POWER_UP);
  delay(100);
  //Play_Group(15,VOLUME);
  delay(100);
  Talk_Back(122);
  while(1);
  // put your main code here, to run repeatedly:

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
  else
  {
    Unit_Digit = Value%10;
    Serial.print("Unit digit");
    Serial.println(Unit_Digit);
    Tenth_Digit = Value/10;
    Tenth_Digit *=10;
    Serial.print("tenth digit");
    Serial.println(Tenth_Digit);
    Play_Group((Tenth_Digit-1),PLAY);
    while(busy_status() == 0);
    Play_Group(Unit_Digit-1,LOAD);
    while(busy_status() == 1);
    while(busy_status() == 0);
    while(busy_status() == 1);
  }
}
