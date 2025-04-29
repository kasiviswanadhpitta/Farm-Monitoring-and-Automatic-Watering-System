//Normal Directories
#include <String.h>
#include <stdlib.h>

//DS18B20 Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#define soilTempPIN 4   // DS18B20 on pin D4 
OneWire oneWire(soilTempPIN);
DallasTemperature DS18B20(&oneWire);
int soilTemp = 0;

// LM394/YL70 Soil Humidity Sensor
#define soilHumPIN 0
int soilHum = 0;

//Light Sensor
#define ldrPIN 10
int light = 0;

//DHT11 Temperature and Humidity Sensor
#include "DHT.h"
int dhtPIN = 11;
int typeofDHT =  DHT11;
DHT dht(dhtPIN, typeofDHT); 
int airTemp = 0;
int airHum = 0;

//switches
#define lampPIN 7
#define pumpPIN 8
boolean pump=0;
boolean lamp=0;

//GSM 
#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(2,3);//rx,tx

String writeKey ="OSM0R0839AGTHV8T";
String readKey = "";

unsigned long delayWriteTime=60000;
unsigned long startWriteTime=0;
unsigned long elapsWriteTime=0;

unsigned long delayReadTime=10000;
unsigned long startReadTime=0;
unsigned long elapsReadTime=0;

bool error=false;
String err="";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gprsSerial.begin(9600);
  pinMode(ldrPIN,INPUT);
  pinMode(pumpPIN,OUTPUT);
  pinMode(lampPIN,OUTPUT);
  digitalWrite(lampPIN,HIGH);
  digitalWrite(pumpPIN,HIGH);
  DS18B20.begin();
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  readSensors();
  elapsWriteTime=millis()-startWriteTime;
  elapsReadTime=millis()-startReadTime;
  if (elapsWriteTime > delayWriteTime){
    displaySensors();
    error=0;
    writeSensors();
    startWriteTime=millis();
  }
  if (error){
    Serial.println(" <<<< ERROR >>>> Reapeating send");
    delay (2000);  
    writeSensors();
  }
  if (elapsReadTime > delayReadTime){
    startReadTime=millis();
  }
  if (soilHum<50){
    digitalWrite(pumpPIN,LOW);
  }
  
}

void readSensors(void)
{  
 /*read all the sensors readings*/
  light = not digitalRead(ldrPIN);
  soilHum = map(analogRead(soilHumPIN), 1023, 0, 0, 100);
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0);           
}

void displaySensors(void)
{ /* Display the values of the Sensors */
  Serial.print ("soilHum   (%): ");
  Serial.println (soilHum);
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
{ // to send the sensors data to thingspeak
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
  getStr += String(soilHum);
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
