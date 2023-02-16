#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_AHTX0.h>
#include <string.h>

#define ADDBUTTON D4

const char* ssid = "Tenda";
const char* password = "1RKKHAPIEJ";


// const char* ssid = "UPC917D5E9";
// const char* password = "7jxkHw2efapT";

unsigned int UdpPort = 1265;
char data_package[255];
char rep_buffer_t[10];
char rep_buffer_h[10];
char rep_buffer[20];
int paczkaDanych;

String date;

WiFiUDP UDP;

Adafruit_AHTX0 aht;

void setup() {
  pinMode(ADDBUTTON,INPUT_PULLUP);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) delay(1);
  UDP.begin(UdpPort);
  aht.begin();
  Serial.begin(9600);
}

void loop() {
  while(true){
    paczkaDanych = UDP.parsePacket();
    if(paczkaDanych){
      int len = UDP.read(data_package, 255);

      if (len > 0) data_package[len] = 0;

      date = data_package;
      Serial.println(digitalRead(ADDBUTTON));
      if(date=="password_temp" && digitalRead(ADDBUTTON) == LOW ){//&& digitalRead(ADDBUTTON) == LOW 
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); // odesłanie do nadawcy
        UDP.write("respond_temp");
        UDP.endPacket(); 
        delay(200);
      }
      else if(date == "pomiar"){
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
     
        dtostrf(temp.temperature, 2, 2, rep_buffer_t);
        dtostrf(humidity.relative_humidity, 2, 2, rep_buffer_h);

        strcpy(rep_buffer,rep_buffer_t);
        strcat(rep_buffer,"/");
        strcat(rep_buffer,rep_buffer_h);

        Serial.print(rep_buffer);
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); // odesłanie do nadawcy
        UDP.write(rep_buffer);
        UDP.endPacket(); 
        delay(200);
        break;
      }
    }
  }
  // ESP.deepSleep(10E6);
}