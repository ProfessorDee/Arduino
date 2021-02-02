#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <MsTimer2.h>
#include <Led4Digits.h>
#include <Wire.h>

#define MEASURE_PERIOD 500  // measurement time, * 2 ms
#define RO  3300  // resistance of the sample resistor, Ohm
#define MIN_ADC 132 // minimum value of the operating range of the ADC
#define MAX_ADC 410 // maximum value of the operating range of the ADC
#define POL_NUM 24  //  number of poles of the characteristic

int sensTable[POL_NUM][2] = {  // table of characteristics of the sensor
  {-55, 490}, {-50, 515}, {-40, 567}, {-30, 624}, {-20, 684}, {-10, 747},
  {0, 815}, {10, 886}, {20, 961}, {25, 1000}, {30,1040}, {40, 1122},
  {50, 1209}, {60, 1299}, {70, 1392}, {80, 1490}, {90, 1591}, {100, 1696},
  {110, 1805}, {120, 1915}, {125, 1970}, {130, 2023}, {140, 2124}, {150, 2211}
};

int codToTempTable[MAX_ADC - MIN_ADC +1];  //  code to temperature conversion table

int timeCount;     // measurement time counter
long  sumA0;       // ADC code summation variable
long  avarageTemp; // average temperature value (sum of ADC codes, average value * 500)
boolean flagTempReady;  // sign of temperature measurement readiness
int temperature;  // calculated temperature, ° C


Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);


void setup()   {
  MsTimer2::set(2, timerInterrupt); // set the timer interrupt period to 2 ms  
  MsTimer2::start();               //  enable timer interrupt
  Serial.begin(9600);
  int codBeginPol=0;  // segment beginning pole code
  int codEndPol;  // segment end pole code
  float koeff;  // coefficient for the interval between the poles

  // pole cycle
  for ( int p= 0; p < (POL_NUM-1); p++ ) {

    // calculate the code for the next pole N= Rx * 1023 / (Re + Rx)
    codEndPol = (int)(((float)sensTable[p+1][1] * 1023.) / (RO + (float)sensTable[p+1][1]) + 0.5) - MIN_ADC ;                
    codToTempTable[codEndPol] = sensTable[p+1][0];  // temperature for the next pole

    // calculate the coefficient for the interval
    koeff = (float)(sensTable[p+1][0] - sensTable[p][0]) / (float)(codEndPol - codBeginPol);

    // temperature interpolation
    for ( int n = codBeginPol; n < codEndPol; n++ ) {
      codToTempTable[n]= sensTable[p][0] + (int)((float)(n - codBeginPol) * koeff +0.5 );
    }
    codBeginPol=codEndPol;    
  }

  //Initialize Display
  display.begin();

  // you can change the contrast around to adapt the display for the best viewing!
  display.setContrast(57);

  // Clear the buffer.
  display.clearDisplay();

  // Display Text
 
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(11, OUTPUT);

}
int pinPLUS = 9;
int pinMINUS = 8;
int desireTemp = 8; 
int btnPlusPRESS;
int btnMinusPRESS;
int coolerSwitch = 11;


void loop() {
  if ( flagTempReady == true ) {
    flagTempReady= false;
    // data is ready

    // calculate temperature
    temperature = (int)((float)avarageTemp / 500. + 0.5);

    // check range
    if (temperature < MIN_ADC) temperature= MIN_ADC;
    if (temperature > MAX_ADC) temperature= MAX_ADC;

    // read the final value from the array
    temperature= codToTempTable[temperature - MIN_ADC];

       
    //increase/decrease the set temperature
    btnPlusPRESS = digitalRead(pinPLUS);
    btnMinusPRESS = digitalRead(pinMINUS);
    
    if (btnPlusPRESS == LOW  && desireTemp<=18)
      {
        desireTemp++;
      }
     else if (btnMinusPRESS == LOW && desireTemp>=8)
      {
        desireTemp--; 
      }   
    
    String temper = "Temp: ";
    String lcdDisp = temper + temperature;
    String setTemp = "Set to:";
    String tempDisp = setTemp + desireTemp;
        
  display.setTextSize(1);
  display.setTextColor(BLACK);
  int testing = 1;
   if (temperature>desireTemp)
    {
      display.setCursor(0, 0);
      display.println(lcdDisp + " " + (char)247 + "C");
      display.setCursor(0, 10);
      display.println(tempDisp + " " + (char)247 + "C");
      display.setCursor(0, 20);
      display.println("Cooler:ON");
      digitalWrite(coolerSwitch, HIGH);
    }
    else if (temperature<=desireTemp)
    {
      display.setCursor(0, 0);
      display.println(lcdDisp + " " +(char)247 + "C");
      display.setCursor(0, 10);
      display.println(tempDisp + " " + (char)247 + "C");
      display.setCursor(0, 20);
      display.println("Cooler:OFF ");
      digitalWrite(coolerSwitch, LOW);
    }
  display.display();
  //delay(2000);
  display.clearDisplay();
  }
}
  //-------------------------------------- interrupt handler 2 ms
void  timerInterrupt() {
  //disp.regen(); // display regeneration

  // measure the average temperature
  timeCount++;  // +1 averaging sample counter
  sumA0+= analogRead(A0);  // sum of the codes for channel A0 of the ADC

  // check the number of averaging samples
  if ( timeCount >= MEASURE_PERIOD ) {
    timeCount= 0;
    avarageTemp= sumA0; // overload of the average value 
    sumA0= 0;
    flagTempReady= true;  // sign of readiness result
    }  
}
