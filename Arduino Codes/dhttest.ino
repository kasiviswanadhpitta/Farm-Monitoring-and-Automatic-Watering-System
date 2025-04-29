//DHT
#include "DHT.h"
#include <stdlib.h>
int pinoDHT = 11;
int tipoDHT =  DHT11;
DHT dht(pinoDHT, tipoDHT); 
int airTemp = 0;
int airHum = 0;

void setup()
{
  Serial.begin(9600); 
  
  dht.begin();
}

void loop()
{
  readSensors();
  displaySensors();
  delay (1000);
}

/********* Read Sensors value *************/
void readSensors(void)
{
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();

  
}

/********* Display Sensors value *************/
void displaySensors(void)
{
  Serial.print ("airTemp  (oC): ");
  Serial.println (airTemp);
  Serial.print ("airHum    (%): ");
  Serial.println (airHum);
  Serial.println ("");
}
