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
//Defino horas de riego Inicio a las 00:01 y termina a las 00:02 (poned las horas a las que hagais las pruebas)
byte horaInicio = 13, minutoInicio = 11;
byte horaFin = 13, minutoFin = 12;
unsigned long time;

//Pantalla 20x4
//Librerias necesarias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

// State machine
int currentState = 0;

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

}

void loop()
{
  // LCD time & data
  displayTime();

  // Main state machine
  switch (currentState) {
    case 0:
      //do something when state equals "NoRiego"
      lcd.setCursor(0, 3); //Saltamos a la segunda linea
      lcd.print("TEST state --> 0");//Escribimos en la primera linea

      lcd.setCursor(0, 1); //Saltamos a la segunda
      lcd.print("--> esperando");//Escribimos en la primera linea
      
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
      lcd.setCursor(0, 1); //Saltamos a la segunda
      lcd.print("--> REGANDO!!");//Escribimos en la primera linea

      digitalWrite (ledPin, HIGH); // activo el led
      myservo.write(180);

      time = time + 1;
      
      // Transition
      if (time > 10) { //comprobamos si es la hora del riego
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
  //Para hacer las comparaciones de cuando empezar y cuando terminar,   lo paso todo a minutos.
  //Por eso momentoInicio = 13*60 + 11 = 791 y de fin = 13*60 + 12 =  792
  //También se podría hacer una doble comparación de horaActual con horaInicio y horaFin
  //y de minutoActual con minutoInicio y minutoFin

  int momentoInicio = (horaInicio * 60) + minutoInicio;
  int momentoFin = (horaFin * 60) + minutoFin;
  int momentoAhora = (hour() * 60) + minute();

  //Esto es que si hemos pasado o estamos en el momento de inicio , pero antes del momento del fin...
  if ((momentoInicio <= momentoAhora) && (momentoAhora < momentoFin)) {
    return true; // estamos en período de riego: devolver “true”
  }
  else {
    return false; // no estamos en período de riego: devolver “false”
  }
}

void displayTime()
{
  // Prepare LCD
  lcd.clear();//Limpiamos la LCD

  // RTC
  DateTime now = rtc.now();
  
  //Show time in lcd
  lcd.setCursor(0, 0); //Saltamos a la tercera
  lcd.print(String(now.day()) + String("/") + String(now.month()) + String("/") + String(now.year()));
  if (((now.month() < 10) && (now.hour() > 10)) || ((now.month() > 10) && (now.hour() < 10)))
  {
    lcd.print("   ");
  }
  else if ((now.month() < 10) && (now.hour() < 10)) 
  {
    lcd.print("    ");
  }
  else
  {
    lcd.print("  ");
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
  // End Show time in lcd
}


