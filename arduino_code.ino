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

// LM394/YL70 Soil Moisture Sensor
#define soilMosPIN 0
int soilMos = 0;

//Light Sensor
#define ldrPIN 5
int light = 0;

//DHT11 Temperature and Humidity Sensor
#include "DHT.h"
int dhtPIN = 6;
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

String channelID="1685530";
String writeKey ="OSM0R0839AGTHV8T";
String readKey = "SGRDVUBYSMZPXEFW";

unsigned long delayWriteTime=50000;
unsigned long startWriteTime=0;
unsigned long elapsWriteTime=0;

unsigned long delayPumpReadTime=500000;
unsigned long startPumpReadTime=0;
unsigned long elapsPumpReadTime=0;

unsigned long delayLampReadTime=600000;
unsigned long startLampReadTime=0;
unsigned long elapsLampReadTime=0;
bool error=0;

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
  displaySensors();
  start:
  error=0;
  elapsWriteTime=millis()-startWriteTime;
  elapsPumpReadTime=millis()-startPumpReadTime;
  elapsLampReadTime=millis()-startLampReadTime;
  
  if (elapsWriteTime > delayWriteTime){
    readSensors();
    writeThingSpeak();
    startWriteTime=millis();
  }
  if (error==1) //Resend if transmission is not completed 
  {       
    Serial.println(" <<<< ERROR >>>>");
    
    delay (2000);  
    goto start; //go to label "start"
  }
  if (elapsPumpReadTime > delayPumpReadTime){
    readThingSpeak("7");
    startPumpReadTime=millis();
  }
  if (elapsLampReadTime > delayLampReadTime){
    readThingSpeak("6");
    startLampReadTime=millis();
  }
  if (soilMos <50){
    if (soilTemp<45){
      pump=1;
    }
  }
  if (soilMos >80){
    pump=0;
  }
  if (not light){
    lamp=1;
  }
  else{
    lamp=0;
  }
  takeActions();
}
void readSensors(void){  
 /*read all the sensors readings*/
  light = not digitalRead(ldrPIN);
  soilMos = map(analogRead(soilMosPIN), 1023, 0, 0, 100);
  airTemp = dht.readTemperature();
  airHum = dht.readHumidity();
  DS18B20.requestTemperatures(); 
  soilTemp = DS18B20.getTempCByIndex(0);           
}
void writeThingSpeak(void){
  startGSM();
  
  // GET Command String
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
  getStr +="&field6=";
  getStr += String(lamp);
  getStr +="&field7=";
  getStr += String(pump);
  
  sendCommand(getStr);
}
void readThingSpeak(String fieldID){
  startGSM();
  //  GET
  String getStr = "GET https://api.thingspeak.com/channels/";
  getStr += channelID;
  getStr +="/fields/";
  getStr += fieldID;
  getStr += "/last?api_key=";
  getStr += readKey;

  char messageDown = sendCommand(getStr);
  if (messageDown=='1')
  {
    if (fieldID=="7"){
      pump=1;
    }
    else if (fieldID=="6"){
      lamp=1;
    }
  }
  else if (messageDown=='0'){
    if (fieldID=="7"){
      pump=0;
    }
    else if (fieldID=="6"){
      lamp=0;
    }
  }
}
void startGSM(void){
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
 
  showSerialData();
 
  gprsSerial.println("AT+CSTT=\"jionet\"");//start task and setting the APN,
  delay(1000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
  if (echoFind("ERROR")){
    error=1;
    return;
  }
  showSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  showSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  showSerialData();
}
void showSerialData(){
  while(gprsSerial.available()!=0)
  Serial.write(gprsSerial.read());
  delay(5000);
}
char saveSerialData(){
  String response="";
  while(gprsSerial.available()!=0){
  char ch = (char)gprsSerial.read();
  response += ch;
  }
  Serial.println("START");
  Serial.println(response[14]);
  Serial.println("STOP");
  return response[14];
}
char sendCommand(String getStr){
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  
  showSerialData();
  
  Serial.println(getStr);
  gprsSerial.println(getStr);
  delay(4000);
  
  showSerialData();
  
  gprsSerial.println((char)26);//sending
  delay(5000);//waiting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  char resp=saveSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  showSerialData();
  return resp;
}
void takeActions(void){
  
  if (pump == 1) digitalWrite(pumpPIN, LOW);
  else digitalWrite(pumpPIN, HIGH);
  if (lamp == 1) digitalWrite(lampPIN, LOW);
  else digitalWrite(lampPIN, HIGH);
}
boolean echoFind(String keyword){
 byte current_char = 0;
 byte keyword_length = keyword.length();
unsigned long deadline = millis() + 5000; // Tempo de espera 5000ms
 while(millis() < deadline){
  if (gprsSerial.available()){
    char ch = gprsSerial.read();
    Serial.write(ch);
    if (ch == keyword[current_char])
      if (++current_char == keyword_length){
       Serial.println();
       return true;
    }
   }
  }
 return false; // Tempo de espera esgotado
}
void displaySensors(void)
{ /* Display the values of the Sensors */
  Serial.print ("soilMos   (%): ");
  Serial.println (soilMos);
  Serial.print ("soilTemp (oC): ");
  Serial.println (soilTemp);
  Serial.print ("light   : ");
  Serial.println (light);
  Serial.print ("airTemp  (oC): ");
  Serial.println (airTemp);
  Serial.print ("airHum    (%): ");
  Serial.println (airHum);
  Serial.print ("Lamp   ");
  Serial.println (lamp);
  Serial.print ("Pump  ");
  Serial.println (pump);
  Serial.println ("");
}
