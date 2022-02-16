#include <Arduino.h>
#include <WiFi.h>


const char *SSID = "Trip";
const char *PWD = "potential";

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  
}

void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
}



