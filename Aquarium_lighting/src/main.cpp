#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


// Przypisanie pinów
#define FLUOLAMP D3 
#define WIFILED D4
#define BLUEPIN  D5 
#define GREENPIN  D6 
#define REDPIN D7   
#define ADDBUTTON D8 


// Dane do sieci wifi
const char* ssid = "Tenda";
const char* password = "1RKKHAPIEJ";

// Port uC
unsigned int udpPort = 7863;

// Odbieranie oraz wysyłanie dancyh
char dataPackage[255];
String date;
int incomingData;

// Obsługa oświetlenia
int blue = 255, red = 255, green = 255;
bool ledMode = false;

WiFiUDP UDP;

void sendMessage();

void setup() {
  Serial.begin(9600);
  // Konfiguracja pwm
  analogWriteRange(255);
  analogWriteFreq(1000);
  analogWrite(REDPIN, 0);
  analogWrite(GREENPIN, 0);
  analogWrite(BLUEPIN, 0);

  // Konfiguracja pinów
  pinMode(FLUOLAMP,OUTPUT); 
  pinMode(ADDBUTTON,INPUT_PULLUP);
  pinMode(WIFILED,OUTPUT);
  digitalWrite(WIFILED,HIGH);

  // Łączenie z wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  delay(1);

  // Uruchomienie protokołu UDP
  UDP.begin(udpPort);
}

void loop(){
  if (WiFi.status() == WL_CONNECTED) digitalWrite(D4,LOW);
  else digitalWrite(D4,HIGH);

  incomingData = UDP.parsePacket();
  if(incomingData){
    int len = UDP.read(dataPackage, 255);
    if (len > 0) dataPackage[len] = 0;

    date = dataPackage;
    Serial.println(date);
    if (date == "r1"){
      analogWrite(BLUEPIN, blue);
      analogWrite(GREENPIN, green);
      analogWrite(REDPIN, red);
      ledMode = true;
      sendMessage();
    }
    else if (date == "r0"){
      analogWrite(BLUEPIN,0);
      analogWrite(GREENPIN, 0);
      analogWrite(REDPIN, 0);
      ledMode = false;
      sendMessage();
    }
    else if(date.substring(0,1) == "r" && ledMode){
      blue = date.substring(1,date.indexOf("g")).toInt();
      green = date.substring(date.indexOf("g")+1,date.indexOf("b")).toInt();
      red = date.substring(date.indexOf("b")+1).toInt();
      analogWrite(BLUEPIN, blue);
      analogWrite(GREENPIN, green);
      analogWrite(REDPIN, red);
      sendMessage();
    }
    else if (date == "s0"){
      digitalWrite(FLUOLAMP,HIGH);
      sendMessage();
    }
    else if (date == "s1"){
      digitalWrite(FLUOLAMP,LOW);
      sendMessage();
    }
    // else if (date == "password_aqua" && digitalRead(ADDBUTTON)==HIGH){
    else if (date == "password_aqua"){
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
        UDP.write("respond_aqua");
        UDP.endPacket();
    }
  }
}

void sendMessageOk(){
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write("OK");
    UDP.endPacket();
}
