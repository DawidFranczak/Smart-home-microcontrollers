#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string.h>

#define LIGHT D5 

// const char* ssid = "Tenda";
// const char* password = "1RKKHAPIEJ";


const char* ssid = "UPC917D5E9";
const char* password = "7jxkHw2efapT";


unsigned int UdpPort = 4324;
char data_package[255];
int paczkaDanych;
String date;
WiFiUDP UDP;

void setup() {
  Serial.begin(9600);
  pinMode(LIGHT,OUTPUT);
  WiFi.begin(ssid,password);
  Serial.printf("Łączę z %s",ssid);
  while (WiFi.status() != WL_CONNECTED) delay(1);
  UDP.begin(UdpPort);
  Serial.printf("IP: %s, port: %d",WiFi.localIP().toString().c_str(),UdpPort);
}

void loop() {
  paczkaDanych = UDP.parsePacket();
  if(paczkaDanych){
    int len = UDP.read(data_package, 255);
    if (len > 0)
    {
      data_package[len] = 0;
    }
    date = data_package;
    Serial.print(date);
    if(date == "password_light"){
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); // odesłanie do nadawcy
      UDP.write("respond_light");
      UDP.endPacket(); 
    }
    else if(date == "change"){
      digitalWrite(LIGHT,!digitalRead(LIGHT));
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); // odesłanie do nadawcy
      if(digitalRead(LIGHT) == LOW){
        UDP.write("ON");
        Serial.print("ON");
      }
      else{
        UDP.write("OFF");
        Serial.print("OFF");
      }
      UDP.endPacket(); 
    }
  }
}