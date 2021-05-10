#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <string.h>

SSD1306AsciiAvrI2c display;

volatile uint8_t counterEnable = 0;
volatile int32_t encStepCount = 0;
char buf[5];
char bufOld[5];

void encChA(){
  if(digitalRead(4) && (counterEnable || (digitalRead(2) && digitalRead(3)))){
    counterEnable = 1;

    if(encStepCount % 2){encStepCount++;}
    else{encStepCount--;}
  }
}

void encChB(){
  if(digitalRead(4) && (counterEnable || (digitalRead(2) && digitalRead(3)))){
    counterEnable = 1;

    if(encStepCount % 2){encStepCount--;}
    else{encStepCount++;}
  }
}

ISR(PCINT2_vect){
  if(!digitalRead(4)){
    counterEnable = 0;
    encStepCount -= (encStepCount % 4);
  }

  if(digitalRead(5)){encStepCount = 0;}
}

void setup(){
  noInterrupts();

  Wire.begin();
  Wire.setClock(400000L);

  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(lcdnumsfullscreen);
  display.clear();

  pinMode(2, INPUT); // Encoder Optocoupler A
  attachInterrupt(digitalPinToInterrupt(2), encChA, CHANGE);
  pinMode(3, INPUT); // Encoder Optocoupler B
  attachInterrupt(digitalPinToInterrupt(3), encChB, CHANGE);
  pinMode(4, INPUT); // SMD Tape Detection Optocoupler
  pinMode(5, INPUT); // Counter Reset Button
  PCMSK2 |= 0x30; // PCINT21 (PD5), PCINT20 (PD4)
  PCICR |= 0x04; // PCIE2 (PCINT23..16)

  interrupts();
}

void loop(){
  sprintf(buf, "%4ld", abs(encStepCount) >> 2);
  //sprintf(buf, "%4ld", abs(encStepCount));

  for(int decPlace = 0; decPlace <= 3; decPlace++){
    if(buf[decPlace] != bufOld[decPlace]){
      display.setCursor(34*decPlace, 0);
      display.print(buf[decPlace]);
    }
  }

  strcpy(bufOld, buf);
}