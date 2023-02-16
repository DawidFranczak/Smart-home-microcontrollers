#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_PWMServoDriver.h>

#define ADDBUTTON D3
#define WIFILED D4
#define SENSORDOWN D5
#define SENSORUP D6

// Połączenie z siecą WiFi
const char* ssid = "Tenda";
const char* password = "1RKKHAPIEJ";

// Port uC
unsigned int udpPort = 2965;

// Odbieranie oraz wysyłani danych
char dataPackage[255];
int incomingData;
String date;

// Ustawienia jasnośći oraz ilości kroków
int brightness = 4096;
int step = 21;
int stairsNumber = 16;

// Czas zapalenia oświetlenia
unsigned long startTime = 0;
unsigned long presentTime = 0;
unsigned long lightingTime = 10000;

// Logika
bool sensorUp = false;
bool sensorDown = false;
bool lightOn = false;
bool on = false;

// Utworzenie obiektu pwm o adresie 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Utworzenie obiektu UDP
WiFiUDP UDP;

void off();
void turnOffSensorDown();
void turnOffSensorUp();
void turnOnSensorDown();
void turnOnSensorUp();
void sendMessageOk();


void setup() {
  Serial.begin(9600);

  // Konfiguracja pinów
  pinMode(WIFILED,OUTPUT);
  // pinMode(SENSORDOWN,INPUT_PULLUP);
  // pinMode(SENSORUP,INPUT_PULLUP);
  // pinMode(ADDBUTTON,INPUT_PULLUP);

  // Wyłączenie diody świadczącej o połączeniu z internetem
  digitalWrite(WIFILED,HIGH);

  // Rozpoczęcie połączenia z internetem 
  WiFi.begin(ssid,password);

  // Uruchomienie protokołu UDP
  UDP.begin(udpPort);

  // Uruchomienie modułu PCA9685
  pwm.begin();

  // Ustawienie częstotliwosći sygnału PWM modułu na 500 Hz
  pwm.setPWMFreq(500);
  
  off();
}

void loop() {

  if(WiFi.status() == WL_CONNECTED)   digitalWrite(WIFILED,LOW);
  else digitalWrite(WIFILED,HIGH);
  
  incomingData = UDP.parsePacket();
  if(incomingData){
    int len = UDP.read(dataPackage, 255);
    if (len > 0) dataPackage[len] = 0;
    date = dataPackage;
    Serial.println(date);

    // if (date == "password_stairs" && digitalRead(ADDBUTTON) == HIGH) { 
    if (date == "password_stairs") { 
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
      UDP.write("respond_stairs");
      UDP.endPacket();
    }
    else if(date =="ON") {
      if(!lightOn) turnOnSensorDown();
      on = true;
      sendMessageOk();
    }
    else if(date =="OFF") {
      if(lightOn) turnOffSensorDown();
      on = false;
      sendMessageOk();
    }
    else if(date.substring(0,2) == "sp"){
      step = 4096/date.substring(2).toInt(); 
      sendMessageOk();
      }
    else if(date.substring(0,2) == "bs") {
      brightness = (4096*date.substring(2).toInt())/100;
      sendMessageOk();
    }
    else if(date.substring(0,2) == "te") {
      lightingTime = 1000*date.substring(2).toInt();
      sendMessageOk();
    }
  }

  // // Załączenie lamp poprzez czujniki
  // if(digitalRead(SENSORDOWN) == HIGH){
  //   delay(20);
  //   if(digitalRead(SENSORDOWN) == HIGH && sensorUp == false && sensorDown == false){
  //     if(!lightOn) {
  //       turnOnSensorDown();
  //       sensorDown = true;
  //       startTime = millis();
  //       }
  //   }
  // }
  // else if(digitalRead(SENSORUP) == HIGH){
  //   delay(20);
  //   if(digitalRead(SENSORUP) == HIGH && sensorUp == false && sensorDown == false){
  //     if(!lightOn) {
  //       turnOnSensorUp();
  //       sensorUp = true;
  //       startTime = millis();
  //     }
  //   }
  // }

  // Odmierzanie czasu do wyłączena oświetlenia
  presentTime = millis();
  if(presentTime - startTime >=lightingTime && sensorDown && on == false ){
    turnOffSensorDown();
  }
  if(presentTime - startTime >=lightingTime && sensorUp && on == false ){
    turnOffSensorUp();
  }


}

void off()
{
  for (int i = 0; i < stairsNumber; i++)
  {
    pwm.setPin(i,0);
  }
}

void turnOffSensorDown(){
  for(int nr = 0; nr < stairsNumber; nr++){
    for(int i = brightness-1; i > 0; i=i-step){
      pwm.setPin(nr,i);
      delay(1);
      if(i <= step) pwm.setPin(nr,0);
    }
  }
  sensorUp = false;
  lightOn = false;
  sensorDown = false;
  delay(2000);
}

void turnOffSensorUp(){
    for(int nr = stairsNumber; nr > -1; nr--){
    for(int i = brightness-1; i > 0; i=i-step){
      pwm.setPin(nr,i);
      delay(1);
      if(i <= step)  pwm.setPin(nr,0);
    }
  }
  sensorUp = false;
  lightOn = false;
  sensorDown = false;
  delay(2000);
}

void turnOnSensorDown(){ 
  for(int nr = 0; nr < stairsNumber; nr++){
    for(int i = 0; i < brightness-1; i=i+step){
      pwm.setPin(nr,i);
      delay(1);
      if(i+step > brightness) pwm.setPin(nr,brightness);
    }
  }
  lightOn = true;
}

void turnOnSensorUp(){
  for(int nr = stairsNumber; nr > -1; nr--){
    for(int i = 0; i < brightness-1; i=i+step){
      pwm.setPin(nr,i);
      delay(1);
      if (i+step > brightness) pwm.setPin(nr,brightness);
    }
  }
  lightOn = true;
}

void sendMessageOk(){
  UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
  UDP.write("OK");
  UDP.endPacket();
}