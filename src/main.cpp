// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <Arduino.h>
#include <stdio.h>

#define TYPE "ZRASZACZ"       //rodzaj urzadzenia
#define SW "1.0"              //wersja programu
#define HW "Nano3"            //wersja sprzetowa

#define LEDBLINK_PIN 13       // I/O pin connected to the LED.
#define LEDBLINK_MS 1000      // Blink rate (in milliseconds).

int pompa = 6;                                  // Pin do przekaznika
//int dioda = 13;                                 // Dioda testowa
int czujnik_r1 = 5;                             // Pin odczytu czujnika ruchu przy wjezdzie
int czujnik_r2 = 4;                             // Pin odczytu czujnika wyjazdu 2
unsigned long odpalenie_pompy = 0;
unsigned long czas_pracy_pompy = 5 * 1000;

boolean pompa_stop_was_send = false;
boolean pompa_start_was_send = false;
boolean czujnik_r1_state = false;
boolean czujnik_r2_state = false;
int sekwencja = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("TYP: " + String(TYPE));
  Serial.println(" HW: " + String(HW));
  Serial.println(" SW: " + String(SW));
  pinMode(czujnik_r1, INPUT);
  pinMode(czujnik_r2, INPUT);
  pinMode(pompa, OUTPUT);
  //pinMode(dioda, OUTPUT);
}

void ledBlink(){
  int ledStatus = LOW;   // Last set LED mode.
  unsigned long ledBlinkTime = 0; // LED blink time. @tidabek nie moze byc static bo po nim piszesz

  // LED blinking heartbeat. Yes, we are alive.
  // For explanation, see:
  // http://playground.arduino.cc/Code/TimingRollover
  if ((long)(millis() - ledBlinkTime) >= 0){
    // Toggle LED.
    ledStatus = (ledStatus == HIGH ? LOW : HIGH);

    // Set LED pin status.
    digitalWrite(LEDBLINK_PIN, ledStatus);

    // Reset "next time to toggle" time.
    ledBlinkTime = millis() + LEDBLINK_MS;
  }
} // End of ledBlink()

void set_pompa(){
  digitalWrite(pompa,HIGH);
  odpalenie_pompy = millis();
  #ifdef DEBUG
  if(pompa_start_was_send == false){
    Serial.println("START POMPY");
        pompa_stop_was_send = false;
        pompa_start_was_send = true;    
  }
  #endif
}

void reset_pompa(){
  unsigned long praca_pompy_do = odpalenie_pompy +  czas_pracy_pompy;
  if (millis() >= praca_pompy_do){
    digitalWrite(pompa, LOW);
    #ifdef DEBUG
      if(pompa_stop_was_send == false){
        Serial.println("STOP POMPY");
        pompa_stop_was_send = true;
        pompa_start_was_send = false;
      }
    #endif
  }
}

void loop() {
  ledBlink(); // Blink the LED to let the user know we are still alive.
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
  // wylacz pompke
  reset_pompa();
}
