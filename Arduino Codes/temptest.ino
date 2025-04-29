// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5 // DS18B20 on pin D5 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int soilTemp = 0;

void setup()
{
  Serial.begin(9600); 
  DS18B20.begin();
}

void loop()
{
  readSensors();
  displaySensors();
  delay (2000);
}
/********* Read Sensors value *************/
void readSensors(void)
{
  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0); // Sensor 0 will capture Soil Temp in Celcius
}

/********* Display Sensors value *************/
void displaySensors(void)
{
  Serial.print ("soilTemp (oC): ");
  Serial.println (soilTemp);
  Serial.println ("");
}
