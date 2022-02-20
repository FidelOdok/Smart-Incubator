#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <math.h>
#include <EEPROM.h>



#define EEPROM_SIZE 100

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif


//Server Connections
const char* mqtt_server = "0657fea3e3c14f5cb989d7bffd886dfd.s1.eu.hivemq.cloud";
const int mqtt_port =8883;
const char* mqtt_username = "smartIncubator";
const char* mqtt_password = "SmartIncubator@1234";

//Message buffer
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (1024)
char msg[MSG_BUFFER_SIZE];
int value =0;

/***
 Device battery Status declerations
***/
float tempreature = 0;
float humidity    = 0;
float humidity    = 0;
float maxPower    = 0;
float heaterOntime = 0;


bool accesspoint = true;
//Time elapsed
int milliseconds=0;

//Configuration Variable
const char *fingerprint PROGMEM = "16 5B 4B A3 A7 8A 59 C6 14 44 CA FF 3E AF F4 7C 70 7D B1 9F";

int ledPin =2;
int dataPin =0;

int pos=0;

WiFiClientSecure espClient;
PubSubClient client(espClient);

bool ispublished;


void setup_wifi() {

    WiFi.mode(WIFI_STA); 
    Serial.begin(115200);
    
    WiFiManager wm;
    //wm.resetSettings();

    bool res;
    res = wm.autoConnect("incubator","Incubator@12345"); // password protected ap

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
      Serial.print("Message arrived [");
      Serial.print(topic);
      Serial.print("] ");
      
      for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
      }
      Serial.println();

      if ((char)payload[0] == '0') {
        Serial.println("OFF");
        digitalWrite(ledPin, HIGH); 
        digitalWrite(dataPin,HIGH); 
      }
      else if ((char)payload[0] == '1') {
        Serial.println("ON");
        digitalWrite(ledPin, LOW);  
         digitalWrite(dataPin,LOW); 
      }
}


void reconnect() {
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "wattflow";
    clientId += String(2345);
    if(client.connect(clientId.c_str(),mqtt_username,mqtt_password,"holder",0,false,"holder",true)) {
      
        Serial.println("wattflow connected");
    
        tempreature=   sin(random(1,10))*40;
        humidity    = 50 + sin(random(1,10))*50;
        humidity    = 50 + sin(random(1,10))*50;
        maxPower    = 300 + sin(random())*100;
        heaterOntime   = ceil(sin(random(1,1000)));
        
        snprintf(msg, MSG_BUFFER_SIZE, "{\"tempreature\":{\"value\":%f,\"unit\":\"C\"},\"humidity\":{\"value\":%f,\"unit\":\"H\"},\"inputVoltage\":{\"value\":%f,\"unit\":\"V\"},\"maxPower\":{\"value\":%f,\"unit\":\"W\"},\"heaterOntime\":{\"value\":%f,\"unit\":\" \"}}", tempreature,humidity,inputVoltage,maxPower,heaterOntime);
        ispublished =   client.publish("data/monitor/incubatorStatus", msg);
        
    
        client.subscribe("2345/control/deviceState");
        client.subscribe("2345/control/deviceSettings");
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
  pinMode(ledPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  client.subscribe("2345/control/deviceState");
  client.subscribe("2345/control/deviceSettings");
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  milliseconds = now;
  if (now - lastMsg > 5000) {
        lastMsg = now;
        if(value<30)value++;
        else value=5;
        tempreature=   sin(random(1,10))*40;
        humidity    = 50 + sin(random(1,10))*50;
        humidity    = 50 + sin(random(1,10))*50;
        maxPower    = 300 + sin(random())*100;
        heaterOntime   = ceil(sin(random(1,1000)));
        snprintf(msg, MSG_BUFFER_SIZE, "{\"tempreature\":{\"value\":%f,\"unit\":\"C\"},\"humidity\":{\"value\":%f,\"unit\":\"H\"},\"inputVoltage\":{\"value\":%f,\"unit\":\"V\"},\"maxPower\":{\"value\":%f,\"unit\":\"W\"},\"heaterOntime\":{\"value\":%f,\"unit\":\" \"}}", tempreature,humidity,inputVoltage,maxPower,heaterOntime);
        ispublished =   client.publish("data/monitor/incubatorStatus", msg);
  }

}
