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
const char* mqtt_server = "44c7696b3bf2419fb93433ab8e5799fa.s1.eu.hivemq.cloud";
const int mqtt_port =8883;
const char* mqtt_username = "Wattflow";
const char* mqtt_password = "Wattflow@4321";

//Message buffer
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (1024)
char msg[MSG_BUFFER_SIZE];
int value =0;

/***
 Device battery Status declerations
***/
float maxVolt=0;
float minVolt=0;
float maxTemp=0;
float minTemp=0;
float remainCap=0;
float totalVolt=0;
float current=0;
float SOC=0;
int numCells=0;
int numString=0;


const char* PARAM_INPUT_1 = "input1";
const char* PARAM_INPUT_2 = "input2";


/***
Cell Voltage Variable
***/
float block1volt=0;
float block2volt=0;
float block3volt=0;
float block4volt=0;
float block5volt=0;
float block6volt=0;
float block7volt=0;
float block8volt=0;
float block9volt=0;
float block10volt=0;
float block11volt=0;
float block12volt=0;
float block13volt=0;
float block14volt=0;

/***
Internal Resistance Variable
***/
float block1Res=0;
float block2Res=0;
float block3Res=0;
float block4Res=0;
float block5Res=0;
float block6Res=0;
float block7Res=0;
float block8Res=0;
float block9Res=0;
float block10Res=0;
float block11Res=0;
float block12Res=0;
float block13Res=0;
float block14Res=0;
/***
Temperature Variable
***/
float temp1=0;
float temp2=0;

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
    clientId += String(deviceID);
    if(client.connect(clientId.c_str(),mqtt_username,mqtt_password,"holder",0,false,"holder",true)) {
      
        Serial.println("wattflow connected");
    
        maxVolt=4.0 + sin(random(1,3));
        minVolt=3.1 + sin(random(1,3));
        maxTemp=27 + sin(random(1,3));
        minTemp=20 + sin(random(1,3));
        remainCap=50 + sin(random(10,30));
        totalVolt=49+ sin(random(1,3));
        current=15 + sin(random(1,3));
        SOC=60 + sin(random(10,30));
        numCells=600;
        numString=14;


        block1volt=4.0 + sin(random(1,3));
        block2volt=4.0 + sin(random(1,3));
        block3volt=4.0 + sin(random(1,3));
        block4volt=4.0 + sin(random(1,3));
        block5volt=3.0 + sin(random(1,4));
        block6volt=4.0 + sin(random(1,4));
        block7volt=3.0 + sin(random(1,4));
        block8volt=3.0 + sin(random(1,4));
        block9volt=3.0 + sin(random(1,3));
        block10volt=3.0 + sin(random(1,4));
        block11volt=4.0 + sin(random(1,3));
        block12volt=3.0 + sin(random(1,4));
        block13volt=4.0 + sin(random(1,3));
        block14volt=3.0 + sin(random(1,4));

        block1Res=1.0 + sin(random(1,3));
        block2Res=1.0 + sin(random(1,3));
        block3Res=1.0 + sin(random(1,3));
        block4Res=0.0 + sin(random(1,3));
        block5Res=0.0 + sin(random(1,4));
        block6Res=0.0 + sin(random(1,4));
        block7Res=0.0 + sin(random(1,4));
        block8Res=0.0 + sin(random(1,4));
        block9Res=0.0 + sin(random(1,3));
        block10Res=0.0 + sin(random(1,4));
        block11Res=0.0 + sin(random(1,3));
        block12Res=001.0 + sin(random(1,4));
        block13Res=00.0 + sin(random(1,3));
        block14Res=0003.0 + sin(random(1,4));

        temp1 = 25.0 + sin(random(1,4));
        temp2 = 25.0 + sin(random(1,4));
        
        snprintf(msg, MSG_BUFFER_SIZE, "{\"maxVolt\":{\"value\":%f,\"unit\":\"v\"},\"minVolt\":{\"value\":%f,\"unit\":\"v\"},\"maxTemp\":{\"value\":%f,\"unit\":\"C\"},\"minTemp\":{\"value\":%f,\"unit\":\"C\"},\"remainCap\":{\"value\":%f,\"unit\":\"Ah\"},\"totalVolt\":{\"value\":%f,\"unit\":\"v\"},\"current\":{\"value\":%f,\"unit\":\"A\"},\"SOC\":%f,\"numCells\":%i,\"numString\":%i,\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}", maxVolt,minVolt,maxTemp,minTemp,remainCap,totalVolt,current,SOC,numCells,numString,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/batteryStatus", msg);
        
        snprintf(msg, MSG_BUFFER_SIZE,"{\"block1\":%f,\"block2\":%f,\"block3\":%f,\"block4\":%f,\"block5\":%f,\"block6\":%f,\"block7\":%f,\"block8\":%f,\"block9\":%f,\"block10\":%f,\"block11\":%f,\"block12\":%f,\"block13\":%f,\"block14\":%f,\"unit\":\"v\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",block1volt,block2volt,block3volt,block4volt,block5volt,block6volt,block7volt,block8volt,block9volt,block10volt,block11volt,block12volt,block13volt,block14volt,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/cellVoltage", msg);

         snprintf(msg, MSG_BUFFER_SIZE,"{\"block1\":%f,\"block2\":%f,\"block3\":%f,\"block4\":%f,\"block5\":%f,\"block6\":%f,\"block7\":%f,\"block8\":%f,\"block9\":%f,\"block10\":%f,\"block11\":%f,\"block12\":%f,\"block13\":%f,\"block14\":%f,\"unit\":\"v\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",block1Res,block2Res,block3Res,block4Res,block5Res,block6Res,block7Res,block8Res,block9Res,block10Res,block11Res,block12Res,block13Res,block14Res,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/internalResistance", msg);


         snprintf(msg, MSG_BUFFER_SIZE,"{\"voltageLow\":%i,\"tempHigh\":%i,\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",true,true,milliseconds,deviceID);
        ispublished =   client.publish("data/Alerts", msg);
     
        snprintf(msg, MSG_BUFFER_SIZE,"{\"temp1\":%f,\"temp2\":%f,\"unit\":\"C\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",temp1,temp2,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/batteryTemperature", msg);
     
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
     
    
        maxVolt=4.0 + sin(random(1,3));
        minVolt=3.1 + sin(random(1,3));
        maxTemp=27 + sin(random(1,3));
        minTemp=20 + sin(random(1,3));
        remainCap=50 + sin(random(10,30));
        totalVolt=49+ sin(random(1,3));
        current=15 + sin(random(1,3));
        SOC=60 + sin(random(10,30));
        numCells=600;
        numString=14;


        
        block1volt=4.0 + sin(random(1,3));
        block2volt=4.0 + sin(random(1,3));
        block3volt=4.0 + sin(random(1,3));
        block4volt=4.0 + sin(random(1,3));
        block5volt=3.0 + sin(random(1,4));
        block6volt=4.0 + sin(random(1,4));
        block7volt=3.0 + sin(random(1,4));
        block8volt=3.0 + sin(random(1,4));
        block9volt=3.0 + sin(random(1,3));
        block10volt=3.0 + sin(random(1,4));
        block11volt=4.0 + sin(random(1,3));
        block12volt=3.0 + sin(random(1,4));
        block13volt=4.0 + sin(random(1,3));
        block14volt=3.0 + sin(random(1,4)); block1Res=1.0 + sin(random(1,3));
        block2Res=1.0 + sin(random(1,3));
        block3Res=1.0 + sin(random(1,3));
        block4Res=0.0 + sin(random(1,3));
        block5Res=0.0 + sin(random(1,4));
        block6Res=0.0 + sin(random(1,4));
        block7Res=0.0 + sin(random(1,4));
        block8Res=0.0 + sin(random(1,4));
        block9Res=0.0 + sin(random(1,3));
        block10Res=0.0 + sin(random(1,4));
        block11Res=0.0 + sin(random(1,3));
        block12Res=001.0 + sin(random(1,4));
        block13Res=00.0 + sin(random(1,3));
        block14Res=0003.0 + sin(random(1,4));

        temp1 = 25.0 + sin(random(1,4));
        temp2 = 25.0 + sin(random(1,4));
        
        snprintf(msg, MSG_BUFFER_SIZE, "{\"maxVolt\":{\"value\":%f,\"unit\":\"v\"},\"minVolt\":{\"value\":%f,\"unit\":\"v\"},\"maxTemp\":{\"value\":%f,\"unit\":\"C\"},\"minTemp\":{\"value\":%f,\"unit\":\"C\"},\"remainCap\":{\"value\":%f,\"unit\":\"Ah\"},\"totalVolt\":{\"value\":%f,\"unit\":\"v\"},\"current\":{\"value\":%f,\"unit\":\"A\"},\"SOC\":%f,\"numCells\":%i,\"numString\":%i,\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}", maxVolt,minVolt,maxTemp,minTemp,remainCap,totalVolt,current,SOC,numCells,numString,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/batteryStatus", msg);
        
        snprintf(msg, MSG_BUFFER_SIZE,"{\"block1\":%f,\"block2\":%f,\"block3\":%f,\"block4\":%f,\"block5\":%f,\"block6\":%f,\"block7\":%f,\"block8\":%f,\"block9\":%f,\"block10\":%f,\"block11\":%f,\"block12\":%f,\"block13\":%f,\"block14\":%f,\"unit\":\"v\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",block1volt,block2volt,block3volt,block4volt,block5volt,block6volt,block7volt,block8volt,block9volt,block10volt,block11volt,block12volt,block13volt,block14volt,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/cellVoltage", msg);

         snprintf(msg, MSG_BUFFER_SIZE,"{\"block1\":%f,\"block2\":%f,\"block3\":%f,\"block4\":%f,\"block5\":%f,\"block6\":%f,\"block7\":%f,\"block8\":%f,\"block9\":%f,\"block10\":%f,\"block11\":%f,\"block12\":%f,\"block13\":%f,\"block14\":%f,\"unit\":\"v\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",block1Res,block2Res,block3Res,block4Res,block5Res,block6Res,block7Res,block8Res,block9Res,block10Res,block11Res,block12Res,block13Res,block14Res,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/internalResistance", msg);


         snprintf(msg, MSG_BUFFER_SIZE,"{\"voltageLow\":%i,\"tempHigh\":%i,\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",1,1,milliseconds,deviceID);
        ispublished =   client.publish("data/Alerts", msg);
     
        snprintf(msg, MSG_BUFFER_SIZE,"{\"temp1\":%f,\"temp2\":%f,\"unit\":\"C\",\"elapseTime\":{\"value\":%i,\"unit\":\"ms\"},\"DeviceID\":%i}",temp1,temp2,milliseconds,deviceID);
        ispublished =   client.publish("data/monitor/batteryTemperature", msg);
  }

}
