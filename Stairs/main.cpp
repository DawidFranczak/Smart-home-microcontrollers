#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Timers.h>
#include <BigNumbers_I2C.h>
#include <Wire.h>
#include <DS1307.h>
#include <PCA9685.h>

#define prawo   10
#define wybierz 9
#define lewo    8

#define czujnik_dol 11
#define czujnik_gora 12

#define drganie_stykow 30

int ile=0,wyb=1,czas_migania=500,ustaw_czas=4000,pom;

int sekundy_v2,minuty_v2,godziny_v2;
int dzien_mies,miesiac,rok,sekundy,minuty,godziny;

bool cz_dol=false,cz_gora=false;
int dl_przejscia=100,czas_swiecenia=10;

bool nawigacja_zegar,nawigacja_menu,nawigacja_zegar_ustawienia,nawigacja_data,nawigacja_data_v2,nawigacja_godzina,nawigacja_godzina_v2,nawigacja_ustawienia;
bool ust_czas_wygaszenia,ust_dzien_mies,ust_mies,ust_rok,ust_sek,ust_min,ust_godz;

bool odswiez=true,flag_mig,wygaszenie_start;

bool flag_czerwona,flag_niebeiska,flag_zielona;
bool nawigacja_ustawienia_trybu,flag_od,flag_do,flag_jasnosc,nawigacja_ustawienia_trybu_v2;
int nr_trybu,jasnosc_wyswietl,zgas_pom=0;

bool ust_dl_zal,ust_dl_zal_v2,flag_dl_pom_sch,flag_dl_swiec,ust_wlaczone=true;

DS1307 clock;

LiquidCrystal_I2C lcd(0x27, 20, 4);
BigNumbers_I2C bigNum(&lcd);
PCA9685 pwm;

void wygas();
void zgas();
void nawigacja();
void zerujaca();
void zegar();
void menu();
void zegar_ustawienia();
void ustawienia_data();
void ustawienia_data_v2();
void ustawienia_godzina();
void ustawienia_godzina_v2();
void ustawienia_wygaszenie();
void zegar_czas();
void ustawienia();
void ustawienia_trybu();
void ustawienia_trybu_v2();
void ustawienia_dl_impulsow();
void ustawienia_dl_impulsow_v2();
//////////////////////////////Deklaracje timerów//////////////////////
Timer wlacz_zegar;
Timer wybor_miganie;
Timer oswietlenie_zgas;

struct swiecenie
{
int od;
int Do;
double jasnosc;
};
swiecenie tryb[3]= {{6,18,1228.8},{18,22,614.4},{22,6,409.6}};

void setup() {

pinMode(prawo,  INPUT_PULLUP); 
pinMode(lewo,   INPUT_PULLUP); 
pinMode(wybierz,INPUT_PULLUP); 
pinMode(czujnik_dol,INPUT);
pinMode(czujnik_gora,INPUT);

lcd.init();
bigNum.begin();
clock.begin();
clock.getTime();
pwm.init();
pwm.setPWMFrequency(500);

lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("Witam");
lcd.setCursor(5, 1);
lcd.print("w");
lcd.setCursor(6, 2);
lcd.print("dupie");
delay(1000);
lcd.clear();
wygas();
menu();

}
void loop() {
nawigacja();
  if (nawigacja_zegar == true)zegar();
  if (wygaszenie_start == true){zegar_czas();}
  if (wlacz_zegar.available())
  {
    zerujaca();
    wygaszenie_start=true;
    wlacz_zegar.time(STOP);
    wybor_miganie.time(STOP);
    lcd.noBacklight();
  if (wybor_miganie.available())
  { 
    if(nawigacja_godzina_v2)
    {
          if(ust_sek  && flag_mig )      {lcd.setCursor(18, 0);lcd.print("  ");flag_mig=false;}
     else if(ust_sek  && flag_mig==false){lcd.setCursor(18, 0);lcd.print("<-");flag_mig=true;}
     else if(ust_min  && flag_mig)       {lcd.setCursor(18, 1);lcd.print("  ");flag_mig=false;}
     else if(ust_min  && flag_mig==false){lcd.setCursor(18, 1);lcd.print("<-");flag_mig=true;}
     else if(ust_godz && flag_mig)       {lcd.setCursor(18, 2);lcd.print("  ");flag_mig=false;}
     else if(ust_godz && flag_mig==false){lcd.setCursor(18, 2);lcd.print("<-");flag_mig=true;}
    }
    if(nawigacja_data_v2)
    {
           if(ust_dzien_mies && flag_mig)       {lcd.setCursor(18, 0);lcd.print("  ");flag_mig=false;}
      else if(ust_dzien_mies && flag_mig==false){lcd.setCursor(18, 0);lcd.print("<-");flag_mig=true;}
      else if(ust_mies       && flag_mig)       {lcd.setCursor(18, 1);lcd.print("  ");flag_mig=false;}
      else if(ust_mies       && flag_mig==false){lcd.setCursor(18, 1);lcd.print("<-");flag_mig=true;}
      else if(ust_rok        && flag_mig)       {lcd.setCursor(18, 2);lcd.print("  ");flag_mig=false;}
      else if(ust_rok        && flag_mig==false){lcd.setCursor(18, 2);lcd.print("<-");flag_mig=true;}
    }
    if(ust_czas_wygaszenia)
    {
      if(flag_mig){lcd.setCursor(14, 2);lcd.print("  ");flag_mig=false;}
      else        {lcd.setCursor(14, 2);lcd.print("<-");flag_mig=true;}
    }
   
    wybor_miganie.restart();
  }
}
  if (wybor_miganie.available())
  { 
    wybor_miganie.begin(czas_migania);
    if(nawigacja_godzina_v2)
    {
          if(ust_sek  && flag_mig )      {lcd.setCursor(18, 0);lcd.print("  ");flag_mig=false;}
     else if(ust_sek  && flag_mig==false){lcd.setCursor(18, 0);lcd.print("<-");flag_mig=true;}
     else if(ust_min  && flag_mig)       {lcd.setCursor(18, 1);lcd.print("  ");flag_mig=false;}
     else if(ust_min  && flag_mig==false){lcd.setCursor(18, 1);lcd.print("<-");flag_mig=true;}
     else if(ust_godz && flag_mig)       {lcd.setCursor(18, 2);lcd.print("  ");flag_mig=false;}
     else if(ust_godz && flag_mig==false){lcd.setCursor(18, 2);lcd.print("<-");flag_mig=true;}
    }
    if(nawigacja_data_v2)
    {
           if(ust_dzien_mies && flag_mig)       {lcd.setCursor(18, 0);lcd.print("  ");flag_mig=false;}
      else if(ust_dzien_mies && flag_mig==false){lcd.setCursor(18, 0);lcd.print("<-");flag_mig=true;}
      else if(ust_mies       && flag_mig)       {lcd.setCursor(18, 1);lcd.print("  ");flag_mig=false;}
      else if(ust_mies       && flag_mig==false){lcd.setCursor(18, 1);lcd.print("<-");flag_mig=true;}
      else if(ust_rok        && flag_mig)       {lcd.setCursor(18, 2);lcd.print("  ");flag_mig=false;}
      else if(ust_rok        && flag_mig==false){lcd.setCursor(18, 2);lcd.print("<-");flag_mig=true;}
    }
    if(ust_czas_wygaszenia)
    {
      if(flag_mig){lcd.setCursor(14, 2);lcd.print("  ");flag_mig=false;}
      else        {lcd.setCursor(14, 2);lcd.print("<-");flag_mig=true;}
    }
    if(nawigacja_ustawienia_trybu_v2)
    {
          if(flag_od      && flag_mig )      {lcd.setCursor(7, 0);lcd.print("  ");flag_mig=false;}
     else if(flag_od      && flag_mig==false){lcd.setCursor(7, 0);lcd.print("<-");flag_mig=true;}
     else if(flag_do      && flag_mig)       {lcd.setCursor(17,0);lcd.print("  ");flag_mig=false;}
     else if(flag_do      && flag_mig==false){lcd.setCursor(17,0);lcd.print("<-");flag_mig=true;}
     else if(flag_jasnosc && flag_mig)       {lcd.setCursor(15,1);lcd.print("  ");flag_mig=false;}
     else if(flag_jasnosc && flag_mig==false){lcd.setCursor(15,1);lcd.print("<-");flag_mig=true;}
    }
    if(ust_dl_zal_v2)
    {
          if(flag_dl_pom_sch && flag_mig )      {lcd.setCursor(18, 0);lcd.print("  ");flag_mig=false;}
     else if(flag_dl_pom_sch && flag_mig==false){lcd.setCursor(18, 0);lcd.print("<-");flag_mig=true;}
     else if(flag_dl_swiec   && flag_mig)       {lcd.setCursor(18, 1);lcd.print("  ");flag_mig=false;}
     else if(flag_dl_swiec   && flag_mig==false){lcd.setCursor(18, 1);lcd.print("<-");flag_mig=true;}
    }
    wybor_miganie.restart();
  }
  if (digitalRead(czujnik_dol) == HIGH  && ust_wlaczone==false && cz_dol==false && cz_gora==false)
  {
    delay(20);
    if(digitalRead(czujnik_dol) == HIGH ){
        cz_dol=true;
         if(clock.hour<tryb[0].Do && clock.hour>tryb[0].od)nr_trybu=0;
    else if(clock.hour<tryb[1].Do && clock.hour>tryb[1].od)nr_trybu=1;
    else if(clock.hour<tryb[2].Do && clock.hour>tryb[2].od)nr_trybu=2;

    for( int nr=0;nr<15;nr++){
    for (double i=0; i<tryb[nr_trybu].jasnosc; i += tryb[nr_trybu].jasnosc/dl_przejscia) {
    pwm.setChannelPWM(nr, i);
      delay(1);
    }
    }
    oswietlenie_zgas.begin(1000);
    }
  }
  if (digitalRead(czujnik_gora)== HIGH  && ust_wlaczone==false && cz_dol==false && cz_gora==false)
  {
    delay(20);
    if(digitalRead(czujnik_gora)== HIGH){
    cz_gora=true;
         if(clock.hour<tryb[0].Do && clock.hour>tryb[0].od)nr_trybu=0;
    else if(clock.hour<tryb[1].Do && clock.hour>tryb[1].od)nr_trybu=1;
    else if(clock.hour<tryb[2].Do && clock.hour>tryb[2].od)nr_trybu=2;

    for( int nr=15;nr>=0;nr--){
    for (double i=0; i<tryb[nr_trybu].jasnosc; i += tryb[nr_trybu].jasnosc/dl_przejscia) {
    pwm.setChannelPWM(nr, i);
      delay(1);
    }
    }
    oswietlenie_zgas.begin(1000);
    }
  }
  if (oswietlenie_zgas.available())
  { 
    zgas_pom++;
    while(zgas_pom==czas_swiecenia) 
    {
      zgas();
      oswietlenie_zgas.time(STOP);
      zgas_pom=0;
      delay(1000);
      break;
    }
    oswietlenie_zgas.restart();
  }
}
void nawigacja()
{
if(nawigacja_menu)
 {
    if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
    if (digitalRead(prawo)    == LOW ){wyb++;menu();while (digitalRead(prawo) == LOW ) {}}
    if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
    if (digitalRead(lewo)     == LOW ){wyb--;menu();while (digitalRead(lewo) == LOW ) {}}
    if (digitalRead(wybierz)  == LOW ){delay(drganie_stykow);}
    if (digitalRead(wybierz)  == LOW ){
      switch (wyb)
      {
      case 1:odswiez = true;wyb = 1;wlacz_zegar.begin(ustaw_czas);ust_wlaczone=false;zegar();break;
      case 2:odswiez = true;wyb = 1;ustawienia();break;
      case 3:odswiez = true;wyb = 1;ustawienia_dl_impulsow();break;
      }
      while (digitalRead(wybierz) == LOW ) {}
   }
 }
/////////////////////////////////////////////////////////////////////////////////////////
if(nawigacja_zegar)
 {
    if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
    if (digitalRead(prawo)    == LOW ){wyb++;wlacz_zegar.restart();zegar();while (digitalRead(prawo) == LOW ) {}}
    if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
    if (digitalRead(lewo)     == LOW ){wyb--;wlacz_zegar.restart();zegar();while (digitalRead(lewo) == LOW ) {}}
    if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
    if (digitalRead(wybierz) == LOW ){wlacz_zegar.restart();
    switch (wyb)
      {
      case 1:odswiez = true;wyb = 1;wlacz_zegar.time(STOP);ust_wlaczone=true;zegar_ustawienia();break;
      case 2:odswiez = true;wyb = 1;wlacz_zegar.time(STOP);ust_wlaczone=true;menu();break;
      }
      while (digitalRead(wybierz) == LOW ) {}
    }
  }
  ////////////////////////////////Zegar ustawienia///////////////////////////////
if(nawigacja_zegar_ustawienia)
 { 
    if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
    if (digitalRead(prawo)    == LOW ){wyb++;zegar_ustawienia();while (digitalRead(prawo) == LOW ) {}}
    if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
    if (digitalRead(lewo)     == LOW ){wyb--;zegar_ustawienia();while (digitalRead(lewo) == LOW ) {}}
    if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
    if (digitalRead(wybierz) == LOW )
    {
      switch (wyb)
      {
      case 1:odswiez=true;wyb = 1;ustawienia_data();break;
      case 2:odswiez=true;wyb = 1;sekundy_v2=clock.second;minuty_v2=clock.minute;godziny_v2=clock.hour;ustawienia_godzina();break;
      case 3:nawigacja_zegar_ustawienia=false;ust_czas_wygaszenia=true;flag_mig=true;wybor_miganie.begin(10);ustawienia_wygaszenie();break;
      case 4:odswiez=true;wlacz_zegar.begin(ustaw_czas);ust_wlaczone=false;zegar();break;
      }
      while (digitalRead(wybierz) == LOW ) {}
    }
 }
  //////////////////////////////Ustawienia data///////////////////////////
if(nawigacja_data) {
  if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
  if (digitalRead(prawo)    == LOW ){wyb++;ustawienia_data();while (digitalRead(prawo) == LOW ) {}}
  if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
  if (digitalRead(lewo)     == LOW ){wyb--;ustawienia_data();while (digitalRead(lewo) == LOW ) {}}
  if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW ){
  switch (wyb)
  {
  case 1: ust_dzien_mies=true;nawigacja_data_v2=true;nawigacja_data=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 2: ust_mies      =true;nawigacja_data_v2=true;nawigacja_data=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 3: ust_rok       =true;nawigacja_data_v2=true;nawigacja_data=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 4: wyb=4; odswiez=true;clock.fillByYMD(rok+2000, miesiac, dzien_mies);clock.setTime();zegar_ustawienia();break;
  }
  while (digitalRead(wybierz) == LOW ) {}
  }
  }
if(nawigacja_data_v2)
 {
  if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
  if (digitalRead(prawo)    == LOW ){delay(80);ustawienia_data_v2();}
  if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
  if (digitalRead(lewo)     == LOW ){delay(80);ustawienia_data_v2();}
  if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW ){
     switch (wyb)
  {
  case 1: nawigacja_data=true;nawigacja_data_v2=false;ust_dzien_mies=false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 0);lcd.print("<-"); break; 
  case 2: nawigacja_data=true;nawigacja_data_v2=false;ust_mies      =false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 1);lcd.print("<-"); break; 
  case 3: nawigacja_data=true;nawigacja_data_v2=false;ust_rok       =false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 2);lcd.print("<-"); break; 
  }
  while (digitalRead(wybierz) == LOW ) {}
 }
 }
 //////////////////////////////Ustawienia godzina//////////////////////////
if(nawigacja_godzina) {
  if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
  if (digitalRead(prawo)    == LOW ){wyb++;ustawienia_godzina();}while (digitalRead(prawo) == LOW ) {}
  if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
  if (digitalRead(lewo)     == LOW ){wyb--;ustawienia_godzina();while (digitalRead(lewo) == LOW ) {}}
  if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW ){
  switch (wyb)
  {
  case 1: ust_sek =true;nawigacja_godzina_v2=true;nawigacja_godzina=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 2: ust_min =true;nawigacja_godzina_v2=true;nawigacja_godzina=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 3: ust_godz=true;nawigacja_godzina_v2=true;nawigacja_godzina=false;flag_mig=true;wybor_miganie.begin(10);break; 
  case 4: wyb=4;odswiez=true;
          if(sekundy_v2!=sekundy) clock.fillByHMS(clock.hour,   clock.minute,sekundy);
          if(minuty_v2!=minuty )  clock.fillByHMS(clock.hour, minuty,   clock.second);
          if(godziny_v2!=godziny) clock.fillByHMS(godziny, clock.minute,clock.second);
          clock.setTime();
          zegar_ustawienia();
          break;
  }
  while (digitalRead(wybierz) == LOW ) {}
  }
 }
if(nawigacja_godzina_v2)
 {
  if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
  if (digitalRead(prawo)    == LOW ){delay(80);ustawienia_godzina_v2();}
  if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
  if (digitalRead(lewo)     == LOW ){delay(80);ustawienia_godzina_v2();}
  if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW ){
     switch (wyb)
  {
  case 1: nawigacja_godzina=true;nawigacja_godzina_v2=false;ust_sek =false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 0);lcd.print("<-"); break; 
  case 2: nawigacja_godzina=true;nawigacja_godzina_v2=false;ust_min =false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 1);lcd.print("<-"); break; 
  case 3: nawigacja_godzina=true;nawigacja_godzina_v2=false;ust_godz=false;flag_mig=false;wybor_miganie.begin(STOP);lcd.setCursor(18, 2);lcd.print("<-"); break; 
  }
  while (digitalRead(wybierz) == LOW ) {}
 }
 }
  /////////////////////////////////Czas wygaszenia/////////////////////
if(ust_czas_wygaszenia)
{
  if (digitalRead(prawo)    == LOW ){delay(drganie_stykow);}
  if (digitalRead(prawo)    == LOW ){delay(80);ustawienia_wygaszenie();}
  if (digitalRead(lewo)     == LOW ){delay(drganie_stykow);}
  if (digitalRead(lewo)     == LOW ){delay(80);ustawienia_wygaszenie();}
  if (digitalRead(wybierz) == LOW ){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW )
  {
    nawigacja_zegar_ustawienia = true;
    ust_czas_wygaszenia = false;
    flag_mig=false;
    wybor_miganie.time(STOP);
    lcd.setCursor(14, 2);
    lcd.print("<-");
    flag_mig=true;
    while(digitalRead(wybierz) == LOW ){};
  }
}
//////////////////////////////////Ustawienia ////////////////////
if(nawigacja_ustawienia)
{
  if (digitalRead(prawo)   == LOW){delay(drganie_stykow);}
  if (digitalRead(prawo)   == LOW){wyb++;ustawienia();while (digitalRead(prawo) == LOW ) {}}
  if (digitalRead(lewo)    == LOW){delay(drganie_stykow);}
  if (digitalRead(lewo)    == LOW){wyb--;ustawienia();while (digitalRead(lewo) == LOW ) {}}
  if (digitalRead(wybierz) == LOW){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW)
  {
    switch (wyb)
      {
      case 1:odswiez = true;wyb=1;nr_trybu=0;ustawienia_trybu();break;
      case 2:odswiez = true;wyb=1;nr_trybu=1;ustawienia_trybu();break;
      case 3:odswiez = true;wyb=1;nr_trybu=2;ustawienia_trybu();break;
      case 4:odswiez = true;wyb=1;menu();break;
      }
    while(digitalRead(wybierz) == LOW ){};
  }
}
////////////////////////////////////Ustawienia trybu/////////////////////
if(nawigacja_ustawienia_trybu)
{
  if (digitalRead(prawo)   == LOW){delay(drganie_stykow);}
  if (digitalRead(prawo)   == LOW){wyb++;ustawienia_trybu();while (digitalRead(prawo) == LOW ) {}}
  if (digitalRead(lewo)    == LOW){delay(drganie_stykow);}
  if (digitalRead(lewo)    == LOW){wyb--;ustawienia_trybu();while (digitalRead(lewo) == LOW ) {}}
  if (digitalRead(wybierz) == LOW){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW)
  {
     switch (wyb)
      {
      case 1:flag_od=true;     nawigacja_ustawienia_trybu=false;nawigacja_ustawienia_trybu_v2=true;wybor_miganie.begin(10);ustawienia_trybu_v2();break;
      case 2:flag_do=true;     nawigacja_ustawienia_trybu=false;nawigacja_ustawienia_trybu_v2=true;wybor_miganie.begin(10);ustawienia_trybu_v2();break;
      case 3:flag_jasnosc=true;nawigacja_ustawienia_trybu=false;nawigacja_ustawienia_trybu_v2=true;wybor_miganie.begin(10);ustawienia_trybu_v2();break;
      case 4:odswiez = true;wyb = 1;ustawienia();break;
      }
    while(digitalRead(wybierz) == LOW ){};
  }
}
////////////////////////////////////Ustawienia trybu_v2/////////////////////
if(nawigacja_ustawienia_trybu_v2)
{
  if (digitalRead(prawo)   == LOW){delay(100);}
  if (digitalRead(prawo)   == LOW){ustawienia_trybu_v2();}
  if (digitalRead(lewo)    == LOW){delay(100);}
  if (digitalRead(lewo)    == LOW){ustawienia_trybu_v2();}
  if (digitalRead(wybierz) == LOW){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW)
  {
       switch (wyb)
      {
      case 1: zerujaca();nawigacja_ustawienia_trybu=true;wybor_miganie.begin(STOP);lcd.setCursor(7, 0);lcd.print("<-");break;
      case 2: zerujaca();nawigacja_ustawienia_trybu=true;wybor_miganie.begin(STOP);lcd.setCursor(17,0);lcd.print("<-");break;
      case 3: zerujaca();nawigacja_ustawienia_trybu=true;wybor_miganie.begin(STOP);lcd.setCursor(15,1);lcd.print("<-");break;
      }
      while(digitalRead(wybierz) == LOW ){};
  }
}
///////////////////////////////////////// ust dl imp//////////////////////////////////
if(ust_dl_zal)
{
  if (digitalRead(prawo)   == LOW){delay(drganie_stykow);}
  if (digitalRead(prawo)   == LOW){wyb++;ustawienia_dl_impulsow();while (digitalRead(prawo) == LOW ) {}}
  if (digitalRead(lewo)    == LOW){delay(drganie_stykow);}
  if (digitalRead(lewo)    == LOW){wyb--;ustawienia_dl_impulsow();while (digitalRead(lewo) == LOW ) {}}
  if (digitalRead(wybierz) == LOW){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW)
  {
    switch (wyb)
      {
      case 1:flag_dl_pom_sch=true;ust_dl_zal=false;ust_dl_zal_v2=true;wybor_miganie.begin(10);break;
      case 2:flag_dl_swiec=true;  ust_dl_zal=false;ust_dl_zal_v2=true;wybor_miganie.begin(10);break;
      case 3:odswiez=true;menu();break;
      }
    while(digitalRead(wybierz) == LOW ){};
    }
}
////////////////////////////////////// ust dl imp _v2 /////////////////////////////
if(ust_dl_zal_v2)
{
  if (digitalRead(prawo)   == LOW){delay(drganie_stykow);}
  if (digitalRead(prawo)   == LOW){ustawienia_dl_impulsow_v2();}
  if (digitalRead(lewo)    == LOW){delay(drganie_stykow);}
  if (digitalRead(lewo)    == LOW){ustawienia_dl_impulsow_v2();}
  if (digitalRead(wybierz) == LOW){delay(drganie_stykow);}
  if (digitalRead(wybierz) == LOW)
  {
    switch (wyb)
      {
      case 1:flag_dl_pom_sch=false;ust_dl_zal=true;ust_dl_zal_v2=false;lcd.setCursor(18,0);lcd.print("<-");wybor_miganie.begin(STOP);break;
      case 2:flag_dl_swiec=false;  ust_dl_zal=true;ust_dl_zal_v2=false;lcd.setCursor(18,1);lcd.print("<-");wybor_miganie.begin(STOP);break;
      case 3:odswiez=true;menu();break;
      }
    while(digitalRead(wybierz) == LOW ){};
    }
}
}
void wygas()
{
   lcd.setCursor(6,3);
        lcd.print("    ");
  for(int i=0;i<16;i++)
  {
    pwm.setChannelOff(i);
  }
}
void zerujaca()
{
  nawigacja_zegar=false;
  odswiez=false;
  nawigacja_menu=false;
  nawigacja_zegar_ustawienia=false;
  ust_czas_wygaszenia=false;
  nawigacja_data=false;
  ust_dzien_mies=false;
  ust_mies=false;
  ust_rok=false;
  nawigacja_data_v2=false;
  nawigacja_godzina=false;
  ust_sek=false;
  ust_min=false;
  ust_godz=false;
  nawigacja_godzina_v2=false;
  wygaszenie_start=false;
  nawigacja_ustawienia=false;
  nawigacja_ustawienia_trybu=false;
  flag_od=false;
  flag_do=false;
  flag_jasnosc=false;
  flag_czerwona=false;
  flag_niebeiska=false;
  flag_zielona=false;
  ust_dl_zal=false;
  ust_dl_zal_v2=false;
  flag_dl_pom_sch=false;
  flag_dl_swiec=false;
  nawigacja_ustawienia_trybu_v2=false;

}
void zegar()
{
   if (odswiez == true)
  {
    zerujaca();
    nawigacja_zegar= true;

    lcd.clear();
    lcd.setCursor(0, 3);
    lcd.print("Ustaw");
    lcd.setCursor(14, 3);
    lcd.print("Menu");
  }
  clock.getTime();
  lcd.setCursor(5, 0);
  if(clock.dayOfMonth<10) {lcd.print("0");lcd.print(clock.dayOfMonth);}
  else lcd.print(clock.dayOfMonth);
  lcd.print("/");
  if(clock.month<10) {lcd.print("0");lcd.print(clock.month);}
  else lcd.print(clock.month);
  lcd.print("/");
  lcd.print(clock.year);
  bigNum.displayLargeNumber(clock.hour / 10, 0, 1);
  bigNum.displayLargeNumber(clock.hour % 10, 3, 1);
  bigNum.displayLargeNumber(clock.minute / 10, 7, 1);
  bigNum.displayLargeNumber(clock.minute % 10, 10, 1);
  bigNum.displayLargeNumber(clock.second / 10, 14, 1);
  bigNum.displayLargeNumber(clock.second % 10, 17, 1);
  lcd.setCursor(6, 1);
  lcd.print(".");
  lcd.setCursor(6, 2);
  lcd.print(".");
  lcd.setCursor(13, 1);
  lcd.print(".");
  lcd.setCursor(13, 2);
  lcd.print(".");

  if (wyb > 2)
    wyb = 1;
  else if (wyb < 1)
    wyb = 2;

  switch (wyb)
  {
  case 1:lcd.setCursor(18,3);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("<-");break;
  case 2:lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(18,3);lcd.print("<-");break;
  }
}
void zegar_czas()
{ 
  clock.getTime();
  lcd.setCursor(5, 0);
  if(clock.dayOfMonth<10) {lcd.print("0");lcd.print(clock.dayOfMonth);}
  else lcd.print(clock.dayOfMonth);
  lcd.print("/");
  if(clock.month<10) {lcd.print("0");lcd.print(clock.month);}
  else lcd.print(clock.month);
  lcd.print("/");
  lcd.print(clock.year);
  bigNum.displayLargeNumber(clock.hour / 10, 0, 1);
  bigNum.displayLargeNumber(clock.hour % 10, 3, 1);
  bigNum.displayLargeNumber(clock.minute / 10, 7, 1);
  bigNum.displayLargeNumber(clock.minute % 10, 10, 1);
  bigNum.displayLargeNumber(clock.second / 10, 14, 1);
  bigNum.displayLargeNumber(clock.second % 10, 17, 1);
  lcd.setCursor(6, 1);
  lcd.print(".");
  lcd.setCursor(6, 2);
  lcd.print(".");
  lcd.setCursor(13, 1);
  lcd.print(".");
  lcd.setCursor(13, 2);
  lcd.print(".");
  if (digitalRead(wybierz) == LOW)
  {
    nawigacja_zegar = true;
    odswiez = true;
    wlacz_zegar.begin(ustaw_czas);
    lcd.backlight();
    zegar();
  }
}
void menu()
{
   if (odswiez == true)
  {
    zerujaca();
    nawigacja_menu=true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zegar");
    lcd.setCursor(0, 1);
    lcd.print("Ustawienia led");
    lcd.setCursor(0, 2);
    lcd.print("Ustawienia wl");
    lcd.setCursor(0, 3);
    lcd.print("");
  }

  if (wyb > 3)wyb = 1;
  else if (wyb < 1)wyb = 3;

  switch (wyb)
  {
  case 1:lcd.setCursor(14,2);lcd.print("  ");lcd.setCursor(15, 1);lcd.print("  ");lcd.setCursor(5, 0);lcd.print("<-");break;
  case 2:lcd.setCursor(5, 0);lcd.print("  ");lcd.setCursor(14, 2);lcd.print("  ");lcd.setCursor(15,1);lcd.print("<-");break;
  case 3:lcd.setCursor(15,1);lcd.print("  ");lcd.setCursor(5,  0);lcd.print("  ");lcd.setCursor(14,2);lcd.print("<-");break;
  }
}
void zegar_ustawienia()
{
  clock.getTime();
  if (odswiez == true)
  {
    zerujaca();
    nawigacja_zegar_ustawienia = true;

    godziny = clock.hour;
    minuty = clock.minute;
    sekundy = clock.second;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data");

    lcd.setCursor(0, 1);
    lcd.print("Godzina");

    lcd.setCursor(0, 2);
    lcd.print("Wygaszanie:");
    lcd.print(ustaw_czas/1000);
    lcd.print("s");

    lcd.setCursor(0, 3);
    lcd.print("Wroc");

    wlacz_zegar.time(STOP);
  }
  if (wyb == 5)
    wyb = 1;
  else if (wyb == 0)
    wyb = 4;

  switch (wyb)
  {
  case 1:lcd.setCursor(7, 1);lcd.print("  ");lcd.setCursor(4, 3);lcd.print("  ");lcd.setCursor(4, 0);lcd.print("<-");break;
  case 2:lcd.setCursor(4, 0);lcd.print("  ");lcd.setCursor(14,2);lcd.print("  ");lcd.setCursor(7, 1);lcd.print("<-");break;
  case 3:lcd.setCursor(4, 3);lcd.print("  ");lcd.setCursor(7, 1);lcd.print("  ");lcd.setCursor(14,2);lcd.print("<-");break;
  case 4:lcd.setCursor(4, 0);lcd.print("  ");lcd.setCursor(14,2);lcd.print("  ");lcd.setCursor(4, 3);lcd.print("<-");break;
  }
}
void ustawienia_data()
{
 if (odswiez == true)
{   
    zerujaca();
    nawigacja_data = true;
    clock.getTime();

    dzien_mies=clock.dayOfMonth;
    miesiac=clock.month;
    rok=clock.year;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dzien mesiaca:");
    lcd.print(dzien_mies);

    lcd.setCursor(0, 1);
    lcd.print("Miesiac:");
    lcd.print(miesiac);

    lcd.setCursor(0, 2);
    lcd.print("Rok:");
    lcd.print(rok);

    lcd.setCursor(0, 3);
    lcd.print("Wroc");
   }
  
      if (wyb==0)wyb=4;
      else if(wyb==5)wyb=1;

    switch (wyb)
    { 
      case 1:lcd.setCursor(18, 3);lcd.print("  ");lcd.setCursor(18, 1);lcd.print("  ");lcd.setCursor(18, 0);lcd.print("<-");break;
      case 2:lcd.setCursor(18, 0);lcd.print("  ");lcd.setCursor(18, 2);lcd.print("  ");lcd.setCursor(18, 1);lcd.print("<-");break;
      case 3:lcd.setCursor(18, 1);lcd.print("  ");lcd.setCursor(18, 3);lcd.print("  ");lcd.setCursor(18, 2);lcd.print("<-");break;
      case 4:lcd.setCursor(18, 2);lcd.print("  ");lcd.setCursor(18, 0);lcd.print("  ");lcd.setCursor(18, 3);lcd.print("<-");break;      
    }
}
void ustawienia_data_v2()
{
   if(ust_dzien_mies==true && digitalRead(prawo)==LOW && dzien_mies<31) {dzien_mies++;lcd.setCursor(14, 0);lcd.print("    ");lcd.setCursor(14, 0);lcd.print(dzien_mies);}
   if(ust_dzien_mies==true && digitalRead(lewo)==LOW && dzien_mies>1) {dzien_mies--;lcd.setCursor(14, 0);lcd.print("    ");lcd.setCursor(14, 0);lcd.print(dzien_mies);}

   if(ust_mies==true && digitalRead(prawo)==LOW && miesiac<12) {miesiac++;lcd.setCursor(8, 1);lcd.print("  ");lcd.setCursor(8, 1); lcd.print(miesiac);}
   if(ust_mies==true && digitalRead(lewo)==LOW && miesiac>1) {miesiac--;lcd.setCursor(8, 1);lcd.print("  ");lcd.setCursor(8, 1); lcd.print(miesiac);}
   
   if(ust_rok==true && digitalRead(prawo)==LOW ){rok++;lcd.setCursor(4, 2);lcd.print("  ");lcd.setCursor(4, 2); lcd.print(rok);}
   if(ust_rok==true && digitalRead(lewo)==LOW ) {rok--;lcd.setCursor(4, 2);lcd.print("  ");lcd.setCursor(4, 2); lcd.print(rok);}
}
void ustawienia_godzina()
{
if (odswiez == true)
  { 
    zerujaca();
    nawigacja_godzina=true;

    clock.getTime();
    sekundy=clock.second;
    minuty =clock.minute;
    godziny=clock.hour;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sekundy:");
    lcd.print(sekundy);

    lcd.setCursor(0, 1);
    lcd.print("Minuty:");
    lcd.print(minuty);

    lcd.setCursor(0, 2);
    lcd.print("Godziny:");
    lcd.print(godziny);

    lcd.setCursor(0, 3);
    lcd.print("Wroc");
   }
  
      if (wyb==0)wyb=4;
      else if(wyb==5)wyb=1;

    switch (wyb)
    { 
      case 1:lcd.setCursor(18, 3);lcd.print("  ");lcd.setCursor(18, 1);lcd.print("  ");lcd.setCursor(18, 0);lcd.print("<-");break;
      case 2:lcd.setCursor(18, 0);lcd.print("  ");lcd.setCursor(18, 2);lcd.print("  ");lcd.setCursor(18, 1);lcd.print("<-");break;
      case 3:lcd.setCursor(18, 1);lcd.print("  ");lcd.setCursor(18, 3);lcd.print("  ");lcd.setCursor(18, 2);lcd.print("<-");break;
      case 4:lcd.setCursor(18, 2);lcd.print("  ");lcd.setCursor(18, 0);lcd.print("  ");lcd.setCursor(18, 3);lcd.print("<-");break;      
    }
}
void ustawienia_godzina_v2()
{
   if(ust_sek==true && digitalRead(prawo)==LOW && sekundy<59){sekundy++;lcd.setCursor(8, 0);lcd.print("    ");lcd.setCursor(8, 0);lcd.print(sekundy);}
   if(ust_sek==true && digitalRead(lewo )==LOW && sekundy>0) {sekundy--;lcd.setCursor(8, 0);lcd.print("    ");lcd.setCursor(8, 0);lcd.print(sekundy);}

   if(ust_min==true && digitalRead(prawo)==LOW && minuty<59) {minuty++;lcd.setCursor(7, 1);lcd.print("  ");lcd.setCursor(7, 1); lcd.print(minuty);}
   if(ust_min==true && digitalRead(lewo) ==LOW && minuty>0)  {minuty--;lcd.setCursor(7, 1);lcd.print("  ");lcd.setCursor(7, 1); lcd.print(minuty);}
   
   if(ust_godz==true && digitalRead(prawo)==LOW && godziny<23){godziny++;lcd.setCursor(8, 2);lcd.print("  ");lcd.setCursor(8, 2); lcd.print(godziny);}
   if(ust_godz==true && digitalRead(lewo) ==LOW && godziny>0) {godziny--;lcd.setCursor(8, 2);lcd.print("  ");lcd.setCursor(8, 2); lcd.print(godziny);}

}
void ustawienia_wygaszenie()
{
if (digitalRead(prawo) == LOW && ustaw_czas < 59000)
  {
    ustaw_czas = ustaw_czas + 1000;

    if (ustaw_czas < 10000)
    {
      lcd.setCursor(11, 2);
      lcd.print("  ");
      lcd.setCursor(11, 2);
      pom = ustaw_czas / 1000;
      lcd.print(pom);
      lcd.print("s");
      lcd.setCursor(13, 2);
      lcd.print(" ");
    }
    else
    {
      lcd.setCursor(11, 2);
      lcd.print("  ");
      lcd.setCursor(11, 2);
      pom = ustaw_czas / 1000;
      lcd.print(pom);
      lcd.print("s");
    }
  }
  if (digitalRead(lewo) == LOW && ustaw_czas >= 1000 )
  {
    ustaw_czas = ustaw_czas - 1000;
    if (ustaw_czas == 0)
      wlacz_zegar.time(STOP);
    if (ustaw_czas < 10000)
    {
      lcd.setCursor(11, 2);
      lcd.print("  ");
      lcd.setCursor(11, 2);
      pom = ustaw_czas / 1000;
      lcd.print(pom);
      lcd.print("s");
      lcd.setCursor(13, 2);
      lcd.print(" ");
    }
    else
    {
      lcd.setCursor(11, 2);
      lcd.print("  ");
      lcd.setCursor(11, 2);
      pom = ustaw_czas / 1000;
      lcd.print(pom);
      lcd.print("s");
    }
  }
}
void ustawienia()
{
  if(odswiez)
  {
    zerujaca();
    nawigacja_ustawienia=true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Przedzial 1");
    lcd.setCursor(0,1);
    lcd.print("Przedzial 2");
    lcd.setCursor(0,2);
    lcd.print("Przedzial 3");
    lcd.setCursor(0,3);
    lcd.print("Wroc");
  }
    if (wyb == 5)
    wyb = 1;
  else if (wyb == 0)
    wyb = 4;

  switch (wyb)
  {
  case 1:lcd.setCursor(11,1);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(11,0);lcd.print("<-");break;
  case 2:lcd.setCursor(11,0);lcd.print("  ");lcd.setCursor(11,2);lcd.print("  ");lcd.setCursor(11,1);lcd.print("<-");break;
  case 3:lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(11,1);lcd.print("  ");lcd.setCursor(11,2);lcd.print("<-");break;
  case 4:lcd.setCursor(11,0);lcd.print("  ");lcd.setCursor(11,2);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("<-");break;
  
  }
}
void ustawienia_trybu()
{
  if(odswiez)
  {
    
    zerujaca();
    nawigacja_ustawienia_trybu=true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Od :");
    lcd.print(tryb[nr_trybu].od);
    lcd.setCursor(10,0);
    lcd.print("do :");
    lcd.print(tryb[nr_trybu].Do);
    lcd.setCursor(0,1);
    lcd.print("Jasnosc:");
    jasnosc_wyswietl=tryb[nr_trybu].jasnosc/40.96;
    lcd.print(jasnosc_wyswietl);
    lcd.print("% ");
    lcd.setCursor(0,3);
    lcd.print("Wroc");
  }
    if (wyb == 5)
    wyb = 1;
  else if (wyb == 0)
    wyb = 4;

  switch (wyb)
  {
  case 1:lcd.setCursor(17,0);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(7, 0);lcd.print("<-");break;
  case 2:lcd.setCursor(7, 0);lcd.print("  ");lcd.setCursor(15,1);lcd.print("  ");lcd.setCursor(17,0);lcd.print("<-");break;
  case 3:lcd.setCursor(17,0);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(15,1);lcd.print("<-");break;
  case 4:lcd.setCursor(7, 0);lcd.print("  ");lcd.setCursor(15,1);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("<-");break;
  }
}
void ustawienia_trybu_v2()
{
if(flag_od && digitalRead(prawo) == LOW) {tryb[nr_trybu].od++;lcd.setCursor(4,0);lcd.print("   ");lcd.setCursor(4, 0);lcd.print(tryb[nr_trybu].od);}
if(flag_od && digitalRead(lewo)  == LOW) {tryb[nr_trybu].od--;lcd.setCursor(4,0);lcd.print("   ");lcd.setCursor(4, 0);lcd.print(tryb[nr_trybu].od);}

if(flag_do && digitalRead(prawo) == LOW) {tryb[nr_trybu].Do++;lcd.setCursor(14,0);lcd.print("   ");lcd.setCursor(14, 0);lcd.print(tryb[nr_trybu].Do);}
if(flag_do && digitalRead(lewo)  == LOW) {tryb[nr_trybu].Do--;lcd.setCursor(14,0);lcd.print("   ");lcd.setCursor(14, 0);lcd.print(tryb[nr_trybu].Do);}

if(flag_jasnosc && digitalRead(prawo) == LOW && tryb[nr_trybu].jasnosc<=4055.04) {tryb[nr_trybu].jasnosc=tryb[nr_trybu].jasnosc+40.96;jasnosc_wyswietl=tryb[nr_trybu].jasnosc/40.96;lcd.setCursor(8,1);lcd.print("    ");lcd.setCursor(8, 1);lcd.print(jasnosc_wyswietl);lcd.print("%");}
if(flag_jasnosc && digitalRead(lewo)  == LOW && tryb[nr_trybu].jasnosc>=40.96)   {tryb[nr_trybu].jasnosc=tryb[nr_trybu].jasnosc-40.96;jasnosc_wyswietl=tryb[nr_trybu].jasnosc/40.96;lcd.setCursor(8,1);lcd.print("    ");lcd.setCursor(8, 1);lcd.print(jasnosc_wyswietl);lcd.print("%");}


}
void ustawienia_dl_impulsow()
{
  if(odswiez)
  {
    zerujaca();
    ust_dl_zal=true;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ilosc krokow: ");
    lcd.print(dl_przejscia);
    lcd.setCursor(0,1);
    lcd.print("Czas zas : ");
    lcd.print(czas_swiecenia);
    lcd.print("s");
    lcd.setCursor(0,3);
    lcd.print("Wroc");
  }
    if (wyb == 4)
    wyb = 1;
  else if (wyb == 0)
    wyb = 3;

  switch (wyb)
  {
  case 1:lcd.setCursor(18,1);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(18,0);lcd.print("<-");break;
  case 2:lcd.setCursor(18,0);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("  ");lcd.setCursor(18,1);lcd.print("<-");break;
  case 3:lcd.setCursor(18,0);lcd.print("  ");lcd.setCursor(18,1);lcd.print("  ");lcd.setCursor(5, 3);lcd.print("<-");break;
  
  }
}
void ustawienia_dl_impulsow_v2()
{
   if(flag_dl_pom_sch==true  && digitalRead(prawo)==LOW )                  {dl_przejscia++;lcd.setCursor(14, 0);lcd.print("   ");lcd.setCursor(14, 0);lcd.print(dl_przejscia);delay(80);}
   if(flag_dl_pom_sch==true  && digitalRead(lewo) ==LOW && dl_przejscia>1) {dl_przejscia--;lcd.setCursor(14, 0);lcd.print("   ");lcd.setCursor(14, 0);lcd.print(dl_przejscia);delay(80);}

   if(flag_dl_swiec==true   && digitalRead(prawo)==LOW )                   {czas_swiecenia++;lcd.setCursor(11, 1);lcd.print("   ");lcd.setCursor(11, 1); lcd.print(czas_swiecenia);lcd.print("s  "); while(digitalRead(prawo)==LOW){}}
   if(flag_dl_swiec==true   && digitalRead(lewo) ==LOW && czas_swiecenia>1){czas_swiecenia--;lcd.setCursor(11, 1);lcd.print("   ");lcd.setCursor(11, 1); lcd.print(czas_swiecenia);lcd.print("s  "); while(digitalRead(lewo )==LOW){}}
}
void zgas()
{
  if(cz_gora)
  {
    for( int nr=15;nr>=0;nr--){
    for (double i =tryb[nr_trybu].jasnosc; i>0; i -= tryb[nr_trybu].jasnosc/dl_przejscia) {
      pwm.setChannelPWM(nr, i);
      if(i<tryb[nr_trybu].jasnosc/10)  pwm.setChannelPWM(nr, 0);
      delay(1);
    }
    }
    cz_gora=false;
  }
  if(cz_dol)
  {
    for( int nr=0;nr<=15;nr++){
    for (double i =tryb[nr_trybu].jasnosc; i>0; i -= tryb[nr_trybu].jasnosc/dl_przejscia) {
      pwm.setChannelPWM(nr, i);
      if(i<tryb[nr_trybu].jasnosc/10)  pwm.setChannelPWM(nr, 0);
      delay(1);
    }
    }
    cz_dol=false;
  }
}
