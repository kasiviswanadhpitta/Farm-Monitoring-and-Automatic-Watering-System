#define ldrPIN 10
int light = 0;
 
void setup()
{
  pinMode(ldrPIN,INPUT);
  Serial.begin(9600); 
}

void loop()
{
  readSensors();
  displaySensors();
  delay (2000);
}
void readSensors(void)
{
  light = digitalRead(ldrPIN); //LDRDark:0  ==> light 1
}
void displaySensors(void)
{
  Serial.print ("light     : ");
  Serial.println (light);
  Serial.println ("");
}
