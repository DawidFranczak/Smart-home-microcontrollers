#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#define WIFILED 1    // Sygnalizcja podłączenia do wifi
#define ADDBUTTON 2     // Przycisk do włączenia oświetlenia
#define BUTTON 3  // Dodanie urządzenia do systemu

// Połączenie z siecą WiFi
const char* ssid = "Nazwa _sieci_wifi";
const char* password = "Hasło_sieci_wifi";

// Porty oraz ip do komunikacji
IPAddress serwerIp(192,168,0,124);
int serwerPort = 6785;

// Port oraz ip uC
unsigned int UdpPort = 7894;
WiFiUDP UDP;

// Odbieranie oraz wysyłani danych
int incomingData;
char dataPackage[255];
String date;

// Ustawienie czasu wciśnięcia przycisku 
unsigned long presenTime;
unsigned long startTime;
unsigned long pressTime = 1000;
bool click = false;


void setup() {

  // Inicjalizacja wejść/wyjść
  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(WIFILED,OUTPUT);
  pinMode(ADDBUTTON,INPUT_PULLUP);

  // Połączenie z siecią wifi oraz uruchomienie protokołu UDP
  digitalWrite(WIFILED,HIGH);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) delay(1);
  UDP.begin(UdpPort);
}

void loop() {
  // Sygnalizacja podłączenia wifi
  if(WiFi.status() == WL_CONNECTED)  digitalWrite(WIFILED,LOW);
  else digitalWrite(WIFILED,HIGH);
  incomingData = UDP.parsePacket();
  if(incomingData){
    int len = UDP.read(dataPackage, 255);
    if (len > 0) dataPackage[len] = 0;
      date = dataPackage;
      
       // Dodanie urządzneia
    if (date == "password_btn" && digitalRead(ADDBUTTON) == HIGH){
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
      UDP.write("respond_btn");
      UDP.endPacket();
      serwerIp = UDP.remoteIP(); 
    } 
  }

  // Włączenie lamp z przycisku 
  if(digitalRead(BUTTON) == LOW && click == false){
    delay(30);
    if(digitalRead(BUTTON) == LOW){
      startTime = millis();
      click = true;
    }
  }
  presenTime = millis();
  if(presenTime - startTime >= pressTime && click ){

  // Włączenie lamp bez limitu czasowego
    if(digitalRead(BUTTON) == LOW){
      UDP.beginPacket(serwerIp, serwerPort);
      UDP.write("still");
      UDP.endPacket(); 
      while(digitalRead(BUTTON) == LOW) delay(1);
      click = false;
    }

    // Włączenie lamp z limitem czasowym 
    else{
      UDP.beginPacket(serwerIp, serwerPort);
      UDP.write("click");
      UDP.endPacket();
      click = false;
    }
  }
}