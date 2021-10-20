//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include<String.h>
#include <Wire.h>
#include "DFRobot_PH.h"
#include <EEPROM.h>
#define SensorPin A0
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40
#define ss 10
#define rst 9
#define dio0 8
//Libraries for LoRa
int pHArray[ArrayLenth];
int pHArrayIndex=0;
const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;
static float pHValue,voltage;
 
#define BAND 915E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
 
//packet counter
int readingID = 0;
 
int counter = 0;
String LoRaMessage = "";
float h;
float t;

 
//Initialize LoRa module
void startLoRA()
{
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if (counter == 10) 
  {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
  LoRa.setSpreadingFactor (12);
  LoRa.setSignalBandwidth(500E3);
  
 }
 
void startsensor()
{
  
}
 
void getReadings(){
//sensor SoilMoisture
float h = soilMoistureValue = analogRead(A0);
Serial.println(" __Data Sensor Terbaca__ ");
Serial.print ("Nilai Sensor : ");
Serial.println(soilMoistureValue);
Serial.print("Kelembaban : ");

soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
if(soilmoisturepercent >= 100)
{
    Serial.println("100 %");
}
else if(soilmoisturepercent <=0)
{
    Serial.println("0 %");
 }
else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
{
  Serial.print(soilmoisturepercent);
  Serial.println("%");
  Serial.println("  ");
}
delay(5000);

//Sensor pH
  float t =  pHValue,voltage;
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  
  if(millis()-samplingTime > samplingInterval)
  {
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
      t = pHValue = (6.4516*voltage)-6.7742;
      samplingTime=millis();
  }
  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.println(" __Data Sensor Terbaca__ ");
    Serial.print("Voltage:");
    Serial.print(voltage,2);
    Serial.print("    pH value: ");
    Serial.println(pHValue,2);

        printTime=millis();
  }
}
double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
    
  }//if
  return avg;
}
 
void sendReadings() {
  LoRaMessage = String(readingID) + "/" + String(soilmoisturepercent) + "%" + String(pHValue,2) ;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
  Serial.println(LoRaMessage);
  
}
 
void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  startLoRA();
  Wire.begin();
  LoRa.setTxPower(20);
  
}
void loop() {
  getReadings();
  sendReadings();
  delay(5000);
}
