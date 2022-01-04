#include <SoftwareSerial.h>

#define RX 11
#define TX 12

String HOST = "api.thingspeak.com";
String PORT = "80";
String AP = "DJ_LAPTOP";
String PASS = "12345678";


String API = "WBAGMD5UG3G64LBX"; 
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";

long last_update=0;

SoftwareSerial esp8266(RX,TX);

int countTrueCommand;
void setup() {
  delay(100);
  Serial.begin(9600);
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  if(countTrueCommand == 1)
  {
  Serial.println("ESP Ok");
  }
  else
  {
    Serial.println("ESP Not Present");
  }

  countTrueCommand = 0;
  sendCommand("AT+CWJAP=\"DJ_LAPTOP\",\"12345678\"",15,"OK");
   if(countTrueCommand == 1)
  {
  Serial.println("ESP Connected to AP");
  }
  else
  {
    Serial.println("ESP Not Connected ");
  }

   countTrueCommand = 0;
   sendCommand("AT+CIPMUX=0",5,"OK");
   if(countTrueCommand == 1)
  {
    Serial.println("ESP Single Connection ");
  }
  else
  {
    Serial.println("ESP CIPMUX not created");
  }
  writeThingSpeak();
  /*
  String getStr = "GET /update?api_key=";
  getStr += API;
  getStr +="&field1=200";
  //getStr += String(temp_f);
  getStr +="&field2=200";
 // getStr += String(humidity);
  getStr +="&field3=200";
  getStr +="&field4=200";
  //getStr += "\r\n\r\n";
  Data_To_Thigspeak(getStr);
  */
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}



void sendCommand(String command, int maxTime, char readReplay[])
{
 int countTimeCommand =0;
 bool found = false;
 
  while(countTimeCommand < (maxTime*1))
  {
  esp8266.println(command);//at+cipsend
  if(esp8266.find(readReplay))//ok
  {
  found = true;
  break;
  }
  countTimeCommand++;
  }
  if(found == true)
  {
  countTrueCommand++;
  }
  if(found == false)
  {
  countTrueCommand = 0;
  }
}



void writeThingSpeak(void)
{
  startThingSpeakCmd();
  // preparacao da string GET
  String getStr = "GET /update?api_key=";
  getStr += API;
  getStr +="&field1=200";
 // getStr += String(temp_f);
  getStr +="&field2=200";
  //getStr += String(humidity);
  getStr += "\r\n\r\n";
  GetThingspeakcmd(getStr); 
}


void startThingSpeakCmd(void)
{
  esp8266.flush();
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "api.thingspeak.com"; // api.thingspeak.com IP address
  cmd += "\",80";

  
  esp8266.println(cmd);
  Serial.print("Start Commands: ");
  Serial.println(cmd);

  if(esp8266.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
}


String GetThingspeakcmd(String getStr)
{
  String cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  esp8266.println(cmd);
  if(esp8266.find(">"))
  {
    esp8266.print(getStr);
    Serial.println(getStr);
    delay(500);
    String messageBody = "";
    while (esp8266.available()) 
    {
      String line = esp8266.readStringUntil('\n');
      if (line.length() == 1) 
      { 
        messageBody = esp8266.readStringUntil('\n');
      }
    }
    Serial.print("MessageBody received: ");
    Serial.println(messageBody);
    return messageBody;
  }
  else
  {
    esp8266.println("AT+CIPCLOSE");     
    Serial.println("AT+CIPCLOSE"); 
  } 
}
