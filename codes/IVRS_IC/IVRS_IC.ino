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
  // put your setup code here, to run once:
}


void loop() {
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
  for (uint16_t i=0x8000;i>0;i=i>>1)
  {
    digitalWrite(CLK_AP,LOW);
    if(i)
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
