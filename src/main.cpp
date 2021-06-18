#include <Arduino.h>

int pompa = 6;                                  // Pin do przekaznika
int dioda = 13;                                 // Dioda testowa
int czujnik_r1 = 5;                             // Pin odczytu czujnika ruchu przy wjezdzie
int czujnik_r2 = 4;                             // Pin odczytu czujnika wyjazdu 2
unsigned long odpalenie_pompy = 0;
unsigned long czas_pracy_pompy = 8 * 1000;

boolean czujnik_r1_state = false;
boolean czujnik_r2_state = false;
int sekwencja = 0;

void setup() {
  Serial.begin(115200);
  pinMode(czujnik_r1, INPUT);
  pinMode(czujnik_r2, INPUT);
  pinMode(pompa, OUTPUT);
  pinMode(dioda, OUTPUT);
}

void set_pompa(){
  digitalWrite(pompa,HIGH);
  odpalenie_pompy = millis();
  #ifdef DEBUG
    Serial.println("START POMPY");
  #endif
}

void reset_pompa(){
  unsigned long praca_pompy_do = odpalenie_pompy +  czas_pracy_pompy;
  if (millis() >= praca_pompy_do){
    digitalWrite(pompa, LOW);
    #ifdef DEBUG
      Serial.println("STOP POMPY");
    #endif
  }
}

void loop() {
  // test czujnika r1
  if(digitalRead(czujnik_r1) == LOW){
    delay(10);
    if (digitalRead(czujnik_r1) == LOW){
      czujnik_r1_state = true;
    }
  }else{
    czujnik_r1_state = false;
  }

  // test czujnika r2
  if (digitalRead(czujnik_r2) == LOW){
    delay(10);
    if (digitalRead(czujnik_r2) == LOW){
      czujnik_r2_state = true;
    }
  }else{
    czujnik_r2_state = false;
  }

  // sekwencja klikniec dla przejazdu od wlasciwej strony
  if(czujnik_r1_state == false && czujnik_r2_state == false){
    sekwencja = 0;
  }
  if(sekwencja == 0 && czujnik_r1_state == true && czujnik_r2_state == false){
    sekwencja = 1;
  }
  if(sekwencja == 1 && czujnik_r1_state == true && czujnik_r2_state == true){
    sekwencja = 2;
  }
  if(sekwencja == 0 && czujnik_r1_state == false && czujnik_r2_state == true){
    sekwencja = 3;
  }

  if(sekwencja == 2){
    set_pompa();
  }

  reset_pompa();
}