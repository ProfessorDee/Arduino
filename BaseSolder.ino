#include <Arduino.h>
#include <TM1637.h>
#include "max6675.h"

#define CLK 2
#define DIO 3

int soPin = 4;// SO=Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin

TM1637 tm(CLK, DIO);

MAX6675 robojax(sckPin, csPin, soPin);




void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}


void setup() {
  pinMode(7, INPUT);
  pinMode(8, OUTPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  tm.init();
  tm.set(2);
  delay(1500);

  Serial.begin(9600);
  
}
int pinPLUS = 10;
int pinMINUS = 9;
int desireTemp; 
int btnPlusPRESS;
int btnMinusPRESS;
int solderTemp;
int setDisplay = 12;
int measureDisplay = 11;
int switchPOS1; // digital input pin 11
int switchPOS2; //digital input pin 12
int numeroUno;
int powerUP = 8;
int solderSWITCH = 7;
int solderDOWN;
int solderSTATE;
int temperature;
bool solderSHUTDOWN = false;
unsigned long timerELAPSED;
unsigned long TimerDisplay = 0;
bool dont = false;
int temper;



void loop() {
  temper = robojax.readCelsius();
  btnPlusPRESS = digitalRead(pinPLUS);
  btnMinusPRESS = digitalRead(pinMINUS);
  switchPOS1 = digitalRead(measureDisplay);
  switchPOS2 = digitalRead(setDisplay);
  solderDOWN = digitalRead(solderSWITCH);
  int desireTemp = analogRead(A0);
  //delay(200);  delay the unstable voltage input
  
  /*if (btnPlusPRESS == LOW)
      {
        desireTemp++;
        //delay(400);
      }
  else if (btnMinusPRESS == LOW)
      {
        desireTemp--;
        //delay(400); 
      }*/
  if (desireTemp > 350)
  {
    desireTemp = 350;
  }
  if (desireTemp < 100)
  {
    desireTemp = 100;
  }

      if (switchPOS1 == LOW)
      {
          numeroUno = temper;
      }
      if (switchPOS2 == LOW)
      {
          numeroUno = desireTemp;
      } 
      if (switchPOS1 == HIGH && switchPOS2 == HIGH)
      {
           numeroUno = TimerDisplay;
      }
      
       displayNumber(numeroUno);     
     

      if (solderDOWN == HIGH && TimerDisplay < 600)
      {
        TimerDisplay = 0; 
      }
      if (solderDOWN == LOW && TimerDisplay < 600)
      {
        TimerDisplay++;
      }
      delay(1000);
      if (TimerDisplay == 600);
      {
        digitalWrite(powerUP, LOW);
        dont = true;
      }
      if (TimerDisplay<600) 
      {
        dont = false;
      }
       if (temper == desireTemp || temper > desireTemp)
      {
          //power down the solder 
          digitalWrite(powerUP, LOW);
      }
      if (temper < desireTemp && dont == false)
      {
        //power up the solder 
        digitalWrite(powerUP, HIGH);
      } 
            

      
      
  
}
