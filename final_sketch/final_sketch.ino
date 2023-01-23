//Mikolaj Pietraszko
//Autopilot Arduino Companion 
//assisting by providing information about distance and temperature 
//via 16x2 LCD display and 8 LED bar


//Libraries
#include <TimerThree.h>
#include <TimerOne.h>
#include <SR04.h>
#include <LiquidCrystal.h>

//Ultrasonic
#define TRIG_PIN 6
#define ECHO_PIN 5
SR04 sr04 =SR04(ECHO_PIN,TRIG_PIN);

long DIST;
long DIST_GLOBAL;

//Temperature Sensor
int TEMP_PIN = 0;



int a=0;
int b=0;

//Shift register for the LED bar
int LATCH_PIN = 3;
int CLOCK_PIN = 2;
int DATA_PIN= 4;

byte LED_BAR=0;

//LCD Display
LiquidCrystal lcd(7,8,9,10,11,12);



void setup() {
  Serial.begin(9600);
  //Ultrasonic Sensor pinmodes
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  
  //Shift Register pinmodes
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  
  //Initialising timer and attaching interrupt
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(INTERRUPT);
  
  lcd.begin(16,2);
  DISP_TEMP(CALC_TEMP ( analogRead ( TEMP_PIN ) ) );
  
  Timer3.initialize(25000);
  Timer3.attachInterrupt(DISTGLOBALFUNCTION);
 
}



void loop() {
  DIST = sr04.Distance();
}



void DISTGLOBALFUNCTION() {
  DIST_GLOBAL = sr04.Distance();
}

  
 
 void DISP_DIST (){
  lcd.setCursor(0,1);
  lcd.print("DISTANCE: ");
  lcd.print(DIST);
  lcd.print("CM");
  
  LED_BAR = 0;
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
  digitalWrite(LATCH_PIN, HIGH);
  }

  

  void DIST_WARNING () {
  lcd.setCursor(0,0);
  lcd.print("TOO CLOSE        ");
  lcd.setCursor(10,1);
  lcd.print(DIST);
  
  LED_BAR = 255;
  
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
  digitalWrite(LATCH_PIN, HIGH);
  
  LED_BAR = 0;
  
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
  digitalWrite(LATCH_PIN, HIGH);
  }



  void DISP_TEMP (float TEMP) {
  lcd.setCursor(0,0);
  lcd.print("TEMP: ");
  lcd.print(TEMP);
  lcd.print(" C");
  }


  
  void LED_TEMP (float TEMP){
  LED_BAR = LED_BAR + ceil(TEMP);
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
  digitalWrite(LATCH_PIN, HIGH);
  LED_BAR=0;
  }

  
    
  void TEMP_WARNING (float TEMP){
    
   DISP_TEMP(TEMP);
   lcd.setCursor(0,1);
   lcd.print("TEMP TOO HIGH   ");
   
   LED_BAR = LED_BAR+ceil(TEMP);
   digitalWrite(LATCH_PIN, LOW);
   shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
   digitalWrite(LATCH_PIN, HIGH);
   
   LED_BAR= 0;
   
   digitalWrite(LATCH_PIN, LOW);
   shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, LED_BAR);
   digitalWrite(LATCH_PIN, HIGH);
    }


    
  float CALC_TEMP (int TEMP_READ) {
    
  //Calculating the temperature in Kelvins and converting to Celsius
  double TEMP_K = log(10000.0 * ((1024.0 / TEMP_READ - 1)));
  TEMP_K = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * TEMP_K * TEMP_K )) * TEMP_K );       
  float TEMP_C = TEMP_K - 273.15;            // Convert Kelvin to Celsius
  return TEMP_C;
  }


  
  void INTERRUPT() {
    
  a++;
  b++;

  //DISPLAYING DISTANCE WARNING ***************************************************************************
  if (DIST<=50) {
  lcd.clear();
  DIST_WARNING();
  DISP_DIST();  
  }

  
  //NORMAL DISPLAYING DISTANCE*****************************************************************************
  else {
  DISP_DIST();
  LED_TEMP(CALC_TEMP (analogRead(TEMP_PIN)));
  }

  
  //DISPLAYING TEMPERATURE WARNING************************************************************************
  if ((b%3==0)&&(CALC_TEMP (analogRead(TEMP_PIN))>=30)){
  lcd.clear();
  TEMP_WARNING (CALC_TEMP (analogRead(TEMP_PIN)));
  b=0;
  }

  
  //NORMAL DISPLAYING TEMPERATURE *************************************************************************
  if (a ==10){
  DISP_TEMP(CALC_TEMP (analogRead(TEMP_PIN)));
  a=0;
  }
  }
