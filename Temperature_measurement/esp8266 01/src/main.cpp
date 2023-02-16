#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

// Zadefiniowani pinów 
#define ADDBUTTON 0
#define DHTPIN 2

// Zdefiniowanie rodzaju wykorzystanego czujnika
#define DHTTYPE DHT11 

// Podanie danych do sieci wifi
const char* ssid = "Nazwa_sieci_wifi";
const char* password = "Hasło_siecie_wifi";

// Zmienne potrzebne do przesyłania danych poprzez UDP
unsigned int udpPort = 1265;
char dataPackage[255];
char repBuffer[10];
int incomingData;
String date;
WiFiUDP UDP;

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  // konifuguracja pinów
  pinMode(ADDBUTTON,INPUT_PULLUP);

  // Połączenie z siecią wifi
  WiFi.begin(ssid,password);

  // Oczekianie na połączenie
  while (WiFi.status() != WL_CONNECTED) delay(1);

  // Uruchomienie protokołu UTP do przesułu danych 
  UDP.begin(udpPort);

  // Uruchomienie czujnika DTH
  dht.begin();
}

void loop() {
  while(true){
    incomingData = UDP.parsePacket();
    if(incomingData){

      int len = UDP.read(dataPackage, 255);
      if (len > 0) dataPackage[len] = 0;
      date = dataPackage;

      // Połączenie modułu z serwerem 
      if(date=="password_temp" && digitalRead(ADDBUTTON) == LOW ){
        // Odesłanie ustalonego hasła do serwera
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
        UDP.write("respond_temp");
        UDP.endPacket(); 
      }
      // Dokonanie pomiaru temperatury
      else if(date == "pomiar"){
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        dtostrf(t, 2, 2, repBuffer);
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
        UDP.write(repBuffer);
        UDP.endPacket(); 
        delay(200);
        break;
      }
    }
  }
  // Wprowadzenie mikrokontorlera w stan głębokiego snu 
  ESP.deepSleep(33E8);
}