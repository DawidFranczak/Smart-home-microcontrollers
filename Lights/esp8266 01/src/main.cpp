#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define LIGHT 0
#define WIFILED 1
#define ADDBUTTON 2


const char* ssid     = "Nazwa_sieci_wifi";
const char* password = "Hasło_sieci_wifi";


unsigned int udpPort = 4324;
char dataPackage[255];
int incomingData;
String date;

WiFiUDP UDP;

void setup() {
  // Konfiguracja pinów
  pinMode(LIGHT,OUTPUT);
  pinMode(WIFILED,OUTPUT);
  pinMode(ADDBUTTON,INPUT_PULLUP);

  // Ustawienie pinów wstan wysoki
  digitalWrite(LIGHT,HIGH);
  digitalWrite(WIFILED,HIGH);

  // Połączenie z siecią wifi
  WiFi.begin(ssid, password);

  // Oczekiwanie na połączenie
  while (WiFi.status() != WL_CONNECTED) delay(1); 

  // Uruchomienie protokołu UTP do przesyłu danych
  UDP.begin(udpPort);
}

void loop() {
  
  // Sprawdzenie aktywnego połączenia z wifi
  if (WiFi.status() == WL_CONNECTED) digitalWrite(WIFILED,LOW);
  else digitalWrite(WIFILED,HIGH);

  incomingData = UDP.parsePacket();
  if(incomingData){

    int len = UDP.read(dataPackage, 255);
    if (len > 0) dataPackage[len] = 0;
    date = dataPackage;

    // Dodanie urządzenia 
    if(date == "password_light" && digitalRead(ADDBUTTON) == HIGH){   
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.write("respond_light");
      UDP.endPacket(); 
    }
    // Zmiana stanu lampy 
    else if(date == "change"){
      digitalWrite(LIGHT,!digitalRead(LIGHT));
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());

      // Sprawdzenie stanu pinu oraz
      // odesłanie wiadomości do serwer 
      // informaci o stanie lampy

      if(digitalRead(LIGHT) == LOW) UDP.write("ON");
      else UDP.write("OFF");
      UDP.endPacket(); 
    }
  }
}