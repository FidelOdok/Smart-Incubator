#include "dimmable_light.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <math.h>
#include <EEPROM.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <Servo.h> 
#include <FastPID.h>


#define EEPROM_SIZE 100

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif


//Server Connections
const char* mqtt_server = "44c7696b3bf2419fb93433ab8e5799fa.s1.eu.hivemq.cloud";
const int mqtt_port =8883;
const char* mqtt_username = "soniclabs";
const char* mqtt_password = "Sonic@54321";

//Message buffer
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (1024)
char msg[MSG_BUFFER_SIZE];
int value =0;

/***
 Device battery Status declerations
***/
const int syncPin = D7;
const int thyristorPin = D5;

int period = 50;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (100)
char msg[MSG_BUFFER_SIZE];
int value =0;
float Sonic_Labs_Incubator_Temperature=0;
float Sonic_Labs_Incubator_Humidity=0;
float Sonic_Labs_Incubator_Fanspeed=0;
float Sonic_Labs_Incubator_InputVoltage=0;
float Sonic_Labs_Incubator_OutputVoltage=0;
int Sonic_Labs_Incubator_Temperature_Recieve=37;
float Sonic_Labs_Incubator_Humidity_Recieve=40;
int milliseconds=0;

float humidity = 0;
float temperature = 0;  

int output_temp_discrete;

float Kp=200, Ki=0.5, Kd=0, Hz=40000;
int output_bits = 8;
bool output_signed = false;

FastPID myPID(Kp, Ki, Kd, Hz, output_bits, output_signed);
bool outputRange = myPID.setOutputRange(0, 255);

int pos=0;

DimmableLight light(thyristorPin);
DHTesp dht;
Servo myservo;

bool accesspoint = true;
//Time elapsed
int milliseconds=0;

//Configuration Variable
const char *fingerprint PROGMEM = "4B 20 57 1B FA 16 8D 63 2B 7F 64 CD 41 92 CB 22 3E 19 FB 77";

int pos=0;

WiFiClientSecure espClient;
PubSubClient client(espClient);

bool ispublished;


void setup_wifi() {

    WiFi.mode(WIFI_STA); 
    Serial.begin(115200);
    
    WiFiManager wm;
    wm.resetSettings();

    bool res;
    res = wm.autoConnect("Wattflow","wattflow@12345"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }

}


void callback(char* topic, byte* payload, unsigned int length) {

}


void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "soniclabs";
    clientId += String(deviceID);
    if(client.connect(clientId.c_str(),mqtt_username,mqtt_password,"holder",0,false,"holder",true)) {
      
        Serial.println("soniclabs connected");

        Sonic_Labs_Incubator_Temperature = temperature;
        Sonic_Labs_Incubator_Humidity=humidity;
        Sonic_Labs_Incubator_Fanspeed=output_temp_discrete;
        Sonic_Labs_Incubator_InputVoltage=228.3;
        Sonic_Labs_Incubator_OutputVoltage=220.5;
        
        
        snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Temperature\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Temperature,milliseconds);
        client.publish("Sonic-Labs-Incubator-Temperature", msg);
        snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Humidity\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Humidity,milliseconds);
        client.publish("Sonic-Labs-Incubator-Humidity", msg);
        snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Fanspeed\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Fanspeed,milliseconds);
        client.publish("Sonic-Labs-Incubator-Fanspeed", msg);
        snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"InputVoltage\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_InputVoltage,milliseconds);
        client.publish("Sonic-Labs-Incubator-InputVoltage", msg);
        snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"OutputVoltage\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_OutputVoltage,milliseconds);
        client.publish("Sonic-Labs-Incubator-OutputVoltage", msg);
        
        client.subscribe("Sonic-Labs-Incubator-Temperature-Recieve");
        client.subscribe("Sonic-Labs-Incubator-Humidity-Recieve");
        client.subscribe("Sonic-Labs-Incubator-P-Recieve");
        client.subscribe("Sonic-Labs-Incubator-I-Recieve");
        client.subscribe("Sonic-Labs-Incubator-D-Recieve");      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  while(!Serial) delay(1);

  Serial.println("All state initialized");

  setup_wifi();
  espClient.setFingerprint(fingerprint);
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  bool sizeBuff= client.setBufferSize(512);
  Serial.println(sizeBuff);

  DimmableLight::setSyncPin(syncPin);
  DimmableLight::begin();
  
  dht.setup(16, DHTesp::DHT11);
  client.subscribe("Sonic-Labs-Incubator-Temperature-Recieve");
  client.subscribe("Sonic-Labs-Incubator-Humidity-Recieve");
  client.subscribe("Sonic-Labs-Incubator-P-Recieve");
  client.subscribe("Sonic-Labs-Incubator-I-Recieve");
  client.subscribe("Sonic-Labs-Incubator-D-Recieve");


  pinMode(D8,OUTPUT);
  randomSeed(micros());
  myservo.write(0);
  myservo.attach(D3);
  delay(100);
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  milliseconds = now;
  if (now - lastMsg > 200) {
      lastMsg = now;
      if(value<30)value++;
      else value=5;
      //delay(dht.getMinimumSamplingPeriod());
      
      // humidity = dht.getHumidity();
      // temperature = dht.getTemperature();  

      humidity = sin(value)+value;
      temperature = cos(value)+value;  

      Sonic_Labs_Incubator_Temperature = temperature;
      Sonic_Labs_Incubator_Humidity=humidity;
      Sonic_Labs_Incubator_Fanspeed=output_temp_discrete;
      Sonic_Labs_Incubator_InputVoltage=228.3 + value;
      Sonic_Labs_Incubator_OutputVoltage=220.5 + value;
      
      
      snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Temperature\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Temperature,milliseconds);
      client.publish("Sonic-Labs-Incubator-Temperature", msg);
      snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Humidity\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Humidity,milliseconds);
      client.publish("Sonic-Labs-Incubator-Humidity", msg);
      snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"Fanspeed\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_Fanspeed,milliseconds);
      client.publish("Sonic-Labs-Incubator-Fanspeed", msg);
      snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"InputVoltage\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_InputVoltage,milliseconds);
      client.publish("Sonic-Labs-Incubator-InputVoltage", msg);
      snprintf(msg, MSG_BUFFER_SIZE, "{\"name\": \"OutputVoltage\",\"value\":%f,\"unit\":\"C\",\"time\":%ld}", Sonic_Labs_Incubator_OutputVoltage,milliseconds);
      client.publish("Sonic-Labs-Incubator-OutputVoltage", msg);
    }
    
    output_temp_discrete = myPID.step(Sonic_Labs_Incubator_Temperature_Recieve,(int)Sonic_Labs_Incubator_Temperature);
    light.setBrightness(output_temp_discrete);
    period = 8000;
    delay(period);

    rotateEggs();
    delay(100);

    if(Sonic_Labs_Incubator_Temperature >Sonic_Labs_Incubator_Temperature_Recieve)digitalWrite(D8,HIGH);
    else digitalWrite(D8,LOW);
  }


void rotateEggs()
{
  for (pos = 0; pos <= 60; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 60; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(150);                       // waits 15ms for the servo to reach the position
  }
}