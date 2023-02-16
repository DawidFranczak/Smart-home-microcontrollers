#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define RST_PIN D0 // Czujnik RFID
#define SS_PIN  D8 // Czujnik RFID

#define BUZZER D1  // Sygnalizacja otwarcia furki 
#define ADDBUTTON D3 // Dodanie urządenia do serwera
#define WIFILED D4 // Sygnalizcja podłączenia do wifi

#define GATE D2    // Furtka
#define OPTO D5    // Domofon

// Połączenie z siecą WiFi
const char* ssid = "Tenda";
const char* password = "1RKKHAPIEJ";

// Czas otwarcia furtki
unsigned long presetTime = 0;
unsigned long startTime = 0;
unsigned long timeOpenGate = 3000;

// Czas do testu działania rfid
unsigned long timeReset = 3000;
unsigned long startResetTime = 60000;

// Czas na dodanie tagu 
unsigned long startAdd = 0;
unsigned long endAdd = 10000;


// Ustawienie jasności
int light = 900;

// Port oraz ip uC
unsigned int localUdpPort = 3984; 
WiFiUDP UDP; 

// Odpbieranie oraz wysyłani danych
char dataPackage[255];
char buff[30];
int paczkaDanych;
String date;

// Port oraz ip do komunikacji
IPAddress serwerIP(192,168,0,124);
int serwerPort = 6785;

// Logika
bool openGate = false;
bool zgoda = false;

// Czujnik RFID
MFRC522 rfid(SS_PIN, RST_PIN); 
String UID = "";


void access(IPAddress serwerIP, int serwerPort);        // Funkcja otwierająca furtkę oraz sprawdzająca jasność oświetlenia słonecznego 
void accessDenied();  // Funkcja dająca sygnał dzwiękowy o braku możliwości otwarcia
void sendUID(String UID, IPAddress serwerIP, int serwerPort); // Wysłanie UID do serwera 
String readUID(); // Odczytanie UID z breloka/karty



void setup() {

  // Inicjalizacja wejść/wyjść
  pinMode(GATE,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(WIFILED,OUTPUT);
  pinMode(OPTO,INPUT_PULLUP);
  digitalWrite(WIFILED,LOW);
  pinMode(ADDBUTTON,INPUT_PULLUP);

  // Uruchomienie czujnika RFID, magistarali SPI, wifi, oraz protokołu UDP 
  SPI.begin(); 
  rfid.PCD_Init(); 
  WiFi.begin(ssid, password);
  UDP.begin(localUdpPort);
  digitalWrite(WIFILED,HIGH);

  while(WiFi.status() != WL_CONNECTED) delay(1);
}

void loop() {
  // Sygnalizacja podłączenia wifis
  if(WiFi.status() == WL_CONNECTED){digitalWrite(WIFILED,LOW);} 
  else digitalWrite(WIFILED,HIGH);

  // Otwarcie furtki poprzez domofon 
  if(digitalRead(OPTO) == LOW){
    delay(20);
    if(digitalRead(OPTO) == LOW) access(serwerIP, serwerPort);
  }
  
  // Odebranie danych 
  paczkaDanych = UDP.parsePacket();
  if(paczkaDanych){
    int len = UDP.read(dataPackage, 255);
    if (len > 0) dataPackage[len] = 0;
      date = dataPackage;
      Serial.print(date); 

      // Dodanie urządzneia
    if (date == "password_rfid" && digitalRead(ADDBUTTON) == HIGH) { 
        UDP.beginPacket(UDP.remoteIP(), UDP.remotePort()); 
        UDP.write("respond_rfid");
        UDP.endPacket();
        serwerIP = UDP.remoteIP(); 
      }
     // Otwarcie furtki poprzez wifi
    else if(date == "access") access(serwerIP, serwerPort);
    else if(date == "access-denied") accessDenied();
    else if(date == "add-tag"){
      startAdd = millis();
      while(true){
        if(rfid.PICC_IsNewCardPresent()){
          UID = readUID();
          Serial.print(UID);
          Serial.println('\n');
          if(UID != ""){
            sendUID(UID,UDP.remoteIP(),UDP.remotePort());
            UID = "";
            break;
          }
        }
        presetTime = millis();
        if (presetTime - startAdd >= endAdd ) break;
      }
    }
  }

  // Otwarcie furtki poprzez kartę rfid
  else if(rfid.PICC_IsNewCardPresent()){
    // Wysłanie UID do serwera
    UID = readUID();
    if(UID != ""){
      sendUID(UID,serwerIP,serwerPort);
    }
    UID = "";
  }

  // Odmierzanie czasu do zamknięcia furtki
  presetTime = millis();
  if(presetTime - startTime >= timeOpenGate && openGate){
    noTone(BUZZER);
    digitalWrite(GATE,LOW);
    openGate = false;
  }
  // Odmierzanie czasu do testu działania czytnika rfid
  if(presetTime - startResetTime >= timeReset){
    rfid.PCD_PerformSelfTest();
    rfid.PCD_Init(); 
    startResetTime = millis();
  }
}

void access(IPAddress serwerIP, int serwerPort){
    if(analogRead(A0)<light){
      UDP.beginPacket(serwerIP, serwerPort);
      UDP.write("RFID");
      UDP.endPacket();
    }
    digitalWrite(GATE,HIGH);
    tone(BUZZER,1000);
    openGate = true;
    startTime = millis();
}

void accessDenied(){
  for(int i = 0; i<4;i++){
  tone(BUZZER,1000);
  delay(200);
  noTone(BUZZER);
  delay(200);
  }
}

String readUID(){
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i<rfid.uid.size; i++){
      UID = UID + rfid.uid.uidByte[i];
    }
    rfid.PICC_HaltA();
  }
  Serial.print(UID);
  Serial.println('\n');
  return UID;
}

void sendUID(String UID, IPAddress serwerIP, int serwerPort){
  UDP.beginPacket(serwerIP, serwerPort);
  UID.toCharArray(buff,30);
  UDP.write(buff);
  UDP.endPacket();
}
