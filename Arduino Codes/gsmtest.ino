#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(2,3);
#include <String.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define soilTempPIN 4   // DS18B20 on pin D4 
OneWire oneWire(soilTempPIN);
DallasTemperature DS18B20(&oneWire);
int soilTemp = 0;

#define soilMosPIN 0
int soilMos = 0;

#define ldrPIN 10
int light = 0;

#include "DHT.h"
#include <stdlib.h>
int dhtPIN = 11;
int typeofDHT =  DHT11;
DHT dht(dhtPIN, typeofDHT); 
int airTemp = 0;
int airHum = 0;

String writeKey ="OSM0R0839AGTHV8T"; 

unsigned long delayTime=60000;
unsigned long startTime=0;
unsigned long elapsTime=0;
bool error=false;
String err="";
void setup()
{
  Serial.begin(9600); 
  gprsSerial.begin(9600);     // the GPRS baud rate   
  pinMode(ldrPIN,INPUT);
  DS18B20.begin();
  dht.begin();
}


void loop()
{ 
  elapsTime=millis()-startTime;
  if (elapsTime > delayTime){
    readSensors();
    displaySensors();
    repeat:
    error=0;
    writeSensors();
    startTime=millis();
  }
  if (error){
    Serial.println(" <<<< ERROR >>>> Reapeating send");
    delay (2000);  
    goto repeat;
  }
  
}
void readSensors(void)
{  
 /*read all the sensors readings*/
  light = not digitalRead(ldrPIN);
  soilMos = map(analogRead(soilMosPIN), 1023, 0, 0, 100);
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0);           
}


void displaySensors(void)
{ /* Display the values of the Sensors */
  Serial.print ("soilMos   (%): ");
  Serial.println (soilMos);
  Serial.print ("soilTemp (oC): ");
  Serial.println (soilTemp);
  Serial.print ("light     : ");
  Serial.println (light);
  Serial.print ("airTemp  (oC): ");
  Serial.println (airTemp);
  Serial.print ("airHum    (%): ");
  Serial.println (airHum);
  Serial.println ("");
}
void ShowSerialData()
{
  while(gprsSerial.available()!=0)
  Serial.write(gprsSerial.read());
  delay(5000);
}
void writeSensors(void)
{
  if (gprsSerial.available())
    Serial.write(gprsSerial.read());
  
  gprsSerial.println("AT");
  delay(1000);
  
  gprsSerial.println("AT+CPIN?");
  delay(1000);
  
  gprsSerial.println("AT+CREG?");
  delay(1000);
  
  gprsSerial.println("AT+CGATT?");
  delay(1000);
  
  gprsSerial.println("AT+CIPSHUT");
  delay(1000);
  
  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);
  
  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CSTT=\"jionet\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
  
  String getStr = "GET https://api.thingspeak.com/update?api_key=";
  getStr += writeKey;
  getStr +="&field1=";
  getStr += String(soilMos);
  getStr +="&field2=";
  getStr += String(soilTemp);
  getStr +="&field3=";
  getStr += String(airTemp);
  getStr +="&field4=";
  getStr += String(airHum);
  getStr +="&field5=";
  getStr += String(light);

  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  
  ShowSerialData();  

  Serial.println(getStr);
  gprsSerial.println(getStr);
  delay(4000);
  ShowSerialData();
  
  gprsSerial.println((char)26);//sending
  delay(5000);//waiting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
}
