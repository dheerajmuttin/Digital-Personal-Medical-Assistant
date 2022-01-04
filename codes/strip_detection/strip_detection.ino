#define STRIP_DETECT 5
void setup() {
  Serial.begin(9600);
  Serial.println("Please Insert The Strip");
  pinMode(STRIP_DETECT,INPUT);
  digitalWrite(STRIP_DETECT,HIGH);
  // put your setup code here, to run once:

}

void loop() {
  int analog_sample;
  if(digitalRead(STRIP_DETECT) == LOW)
  {
    Serial.println("Streap Detetected");
    delay(1000);
    analog_sample = analogRead(A1);
    Serial.print("Blood sample data = ");
    Serial.println(analog_sample);
  }
  // put your main code here, to run repeatedly:

}
