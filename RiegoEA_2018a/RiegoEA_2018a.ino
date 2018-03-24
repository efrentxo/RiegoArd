// Auto Riego

#include <Time.h>   //usamos la librería Time.h 
#include <Servo.h>  //usamos la librería Servo.h

//Using RTC
#include <RTClib.h> //usamos la librería RTClib.h for the RTC module 
RTC_DS3231 rtc;
String daysOfTheWeek[7] = { "Sunday", "Monday", "Thuesday", "Wednesday", "Thrusday", "Friday", "Saturday" };
String monthsNames[12] = { "January", "February", "March", "April", "May", "June", "July", "Agost","September","October","November","December" };
 
Servo myservo;  // create servo object to control the valve
const int ledPin = 12; // pin for the led indicator

// Parameters:
const int  TimeWatering = 300; // [sec]
const int hora = 0;
unsigned long time;
unsigned long timerday;

// Define initial hour and minute for watering
// Program 1
int Program1 = 1;       // 1:enable, 0:disable
byte HourInit1 = 22;    // Initial hour to start watering  
byte MinuteInit1 = 10;  // Initial minutes to start watering
byte dayofweek1 = 7;    // Day of the week to water

// Program 2  
int Program2 = 1;       // 1:enable, 0:disable
byte HourInit2 = 22;  
byte MinuteInit2 = 04;
byte dayofweek2 = 7;

//Librerias necesarias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //Pantalla 20x4

// State machine
int currentState = 0;

// Sensor Temperatura DS18B20
#include <DallasTemperature.h>
#include <OneWire.h>
  // DQ esta conectado al pin 8 de arduino //
#define ONE_WIRE_BUS 2
  // Configuramos el pin asignado al sensor 1-Wire a DallasTemperature//
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensores(&oneWire);
float val=0;        // variable donde guardaremos la temperatura leida del sensor

void setup()
{
  pinMode(ledPin, OUTPUT);   // led indicator
  myservo.attach(9);         // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  //Pongo el reloj en la hora actual (en mi caso las 13:10:00 del 21/11/2013)
  //con setTime(hours, minutes, seconds, days, months, years);
  //setTime(13, 10, 00, 21, 11, 2018);

  //Iniciamos el fondo retroiluminado
  lcd.backlight();
  //Iniciamos la pantalla
  lcd.init();

  // Update date & time with compiling info, only needed the first time RTC is used
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  time = 0;
  timerday = 0;

}

void loop()
{
  // LCD time & data
  displayTime();
  displayTemp();
  
  // Main state machine
  switch (currentState) {
    case 0:
      //do something when state equals "NoRiego"
      lcd.setCursor(0, 3); //Saltamos a la segunda linea
      lcd.print("TEST state --> 0");//Escribimos en la primera linea

      
      digitalWrite (ledPin, LOW); // cierro el led
      myservo.write(0);

      // Transition
      if (esHoraRiego()) { //comprobamos si es la hora del riego
        currentState = 1;
      }
      break;

    case 1:
      //do something when state equals "RiegoProg"
      lcd.setCursor(0, 3); //Saltamos a la segunda linea
      lcd.print("TEST state --> 1");//Escribimos en la primera linea
      lcd.setCursor(0, 2); //Saltamos a la segunda
      lcd.print("--> REGANDO!!");//Escribimos en la primera linea

      digitalWrite (ledPin, HIGH); // activo el led
      myservo.write(180);

      time = time + 1;
      
      // Transition
      if (time > 60) { //comprobamos si es la hora del riego
        currentState = 0;
        time = 0;
      }

      break;

    default:
      // if nothing else matches, do the default
      // default is optional
      lcd.print("State default...");//Escribimos en la primera linea
      break;
  }


  // time_program = millis();
  //prints time since program started
 
  delay(1000); // espero 1s
}

boolean esHoraRiego() {
  DateTime now = rtc.now();
  
  // Check if the time is equal to program watering
  // Minimum 60sec, less thatn 60sec it is not possible
  // Check program 1:
  if (Program1 == 1 && HourInit1 == now.hour() && MinuteInit1 == now.minute())
  {
    return true; // estamos en período de riego: devolver “true”
  }
  else
  {
    return false; // no estamos en período de riego: devolver “false”
  }
}

void displayTime()
{
  // Prepare LCD
  // lcd.clear();//Limpiamos la LCD

  // RTC
  DateTime now = rtc.now();
  
  if (timerday < 5)
  {
      //Show date in lcd
      lcd.setCursor(0, 0); //Init cursor position
      lcd.print(String(now.day()) + String("/") + String(now.month()) + String("/") + String(now.year()));
  }
  else
  {
     // Show day of the week
     lcd.setCursor(0,0);
     lcd.print(daysOfTheWeek[now.dayOfTheWeek()] + String("  ")); 
     if (timerday > 10)
     {
        timerday = 0;
     }
  }
  timerday = timerday + 1;
  
  if (now.hour() < 10)
  {
    lcd.setCursor(13,0);
  }
  else
  {
    lcd.setCursor(12,0);
  }
  
  if ((now.second() < 10) && (now.minute() < 10))
  {
    lcd.print(String(now.hour()) + String(":0") + String(now.minute()) + String(":0") + String(now.second()));
  }
  else if (now.minute() < 10)
  {
    lcd.print(String(now.hour()) + String(":0") + String(now.minute()) + String(":") + String(now.second()));
  }
  else if (now.second() < 10)
  {
    lcd.print(String(now.hour()) + String(":") + String(now.minute()) + String(":0") + String(now.second()));
  }
  else
  {
    lcd.print(String(now.hour()) + String(":") + String(now.minute()) + String(":") + String(now.second()));
  }

  // Show watering program 1
  if (Program1 == 1)
  {
     lcd.setCursor(0,2); 
     lcd.print(String("WATERING AT ") + String(HourInit1) + String(":") + String(MinuteInit1) + String(":") + String("00"));
  }
  // End Show time in lcd
}

void displayTemp()
{
  // Send the comand to obtain the temperature
  sensores.requestTemperatures(); 
  // Save temperature value in the "val" variable
  float val =  sensores.getTempCByIndex(0);
  // Set lcd cursor in line 2
  lcd.setCursor(0,1); 
  // Write in lcd temperature value and ºC simbol
  lcd.print(String(val) + String(char(223))+ String("C"));
}

