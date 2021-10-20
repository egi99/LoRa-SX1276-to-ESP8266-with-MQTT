// Import Wi-Fi library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
 
//define the pins used by the LoRa transceiver module
#define ss 15
#define rst 16
#define dio0 2
 
#define BAND 915E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
 
 
const char* ssid = "jasver";
const char* password =  "jasver03";
const char* mqttServer = "hairdresser.cloudmqtt.com";
const int mqttPort = 17539;
const char* mqttUser = "jcueupfz";
const char* mqttPassword = "UUoOwBP5jdfI";
WiFiClient espClient;
PubSubClient client(espClient);
 
// Initialize variables to get and save LoRa data
int rssi;
String loRaMessage;
String temperature;
String humidity;
String readingID;
 
 
// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE")
  {
    return temperature;
  }
  else if(var == "HUMIDITY")
  {
    return humidity;
  }
  else if (var == "RRSI")
  {
    return String(rssi);
  }
  return String();
}
 
void setup() {
  Serial.begin(115200);
  int counter;
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module
   LoRa.setSpreadingFactor (8);
  LoRa.setSignalBandwidth(250E3);
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("connected"); 
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.publish("esp/test", "Hello from ESP8266");
  client.subscribe("esp/test");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}
 
// Read LoRa packet and get the sensor readings
void loop() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    Serial.print("Lora packet received: ");
    while (LoRa.available())    // Read packet
  {
    String LoRaData = LoRa.readString();
//    Serial.print(LoRaData); 
    
    int pos1 = LoRaData.indexOf('/');   
    int pos2 = LoRaData.indexOf('&');   
    readingID = LoRaData.substring(0, pos1);                   // Get readingID
    temperature = LoRaData.substring(pos1 +1, pos2);           // Get temperature
    humidity = LoRaData.substring(pos2+1, LoRaData.length());  // Get humidity
  }
  
  rssi = LoRa.packetRssi();       // Get RSSI
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
  client.publish("RSSI", String(rssi).c_str());
}

  Serial.print(temperature);
  Serial.print("-");
  Serial.println(humidity);
  client.publish("pH", String(temperature).c_str());
  client.publish("kelembapan", String(humidity).c_str());
  delay(500);
  client.loop();

}
