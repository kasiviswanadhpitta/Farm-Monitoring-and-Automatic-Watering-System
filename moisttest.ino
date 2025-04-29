#define soilHumPIN 0
int soilHum = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensors();
  displaySensors();
  delay (2000);
}
void readSensors(void)
{  
  soilHum = map(analogRead(soilHumPIN), 1023, 0, 0, 100);           
}

/********* Display Sensors value *************/
void displaySensors(void)
{ Serial.print ("soilHum   (%): ");
  Serial.println (soilHum);
  Serial.println ("");
}
