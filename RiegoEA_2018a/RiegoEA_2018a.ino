
// Auto Riego

#include <Time.h> //usamos la librería Time.h 
#include <Servo.h> //usamos la librería Servo.h 

Servo myservo;  // create servo object to control the valve
const int ledPin = 12; // pin for the led indicator

//Defino horas de riego Inicio a las 00:01 y termina a las 00:02 (poned las horas a las que hagais las pruebas)
byte horaInicio = 13, minutoInicio = 11;
byte horaFin = 13, minutoFin = 12;
unsigned long time;
//const int  TimeWatering = 300;
const int hora =0;

//Pantalla 20x4
//Librerias necesarias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  pinMode(ledPin, OUTPUT);   // led indicator
  myservo.attach(9);         // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  //Pongo el reloj en la hora actual (en mi caso las 13:10:00 del 21/11/2013)
  //con setTime(hours, minutes, seconds, days, months, years);
  setTime(13, 10, 00, 21, 11, 2018);

  //Iniciamos el fondo retroiluminado
  lcd.backlight();
  //Iniciamos la pantalla
  lcd.init();

}

void loop()
{
  lcd.clear();//Limpiamos la LCD
  lcd.print("TEST...");//Escribimos en la primera linea
  lcd.setCursor(0, 1); //Saltamos a la segunda linea
  // Actualiza el estado de la maquina

  //switch (currentState)
  //{
  // case NoRiego:
  /// Codigo No Riego standby
  //    lcd.print("NO ES HORA DE RIEGO");//Escribimos en la primera linea
  //      lcd.setCursor(0,2);//Saltamos a la segunda

  // Transitions
  //    if(esHoraRiego()){ //comprobamos si es la hora del riego
  //      currentState = RiegoAuto;
  //     }
  //break;

  //case RiegoAuto:
  /// Codigo RIego auto
  //      lcd.print("ES HORA DE RIEGO");//Escribimos en la primera linea
  //     lcd.setCursor(0,2);//Saltamos a la segunda


  //  break;

  //  case RiegoManual:


  // break;
  // }


  if (esHoraRiego()) { //comprobamos si es la hora del riego
    digitalWrite (ledPin, HIGH); // activo el led
    lcd.print("--> ES HORA DE RIEGO!!");//Escribimos en la primera linea
    lcd.setCursor(0, 1); //Saltamos a la segunda
    myservo.write(180);
  }
  else {
    Serial.println(" --> No es hora riego"); // no es hora de riego: informo por serie
    digitalWrite (ledPin, LOW); // cierro el led
    lcd.print("--> No es hora riego");//Escribimos en la primera linea
  
    myservo.write(0);
  }

  //Show time in lcd
  time_t t = now();
  lcd.setCursor(0, 2); //Saltamos a la tercera
  lcd.print(String(day(t)) + String(":") + String(month(t)) + String(":") + String(year(t)));
  lcd.setCursor(0, 3); //Saltamos a la cuarta
  if (second(t) < 10)
  {
    lcd.print(String(hour(t)) + String(":") + String(minute(t)) + String(":0") + String(second(t)));
  }
  else
  {
    lcd.print(String(hour(t)) + String(":") + String(minute(t)) + String(":") + String(second(t)));
  }
  // End Show time in lcd

  
  time = millis();
  //prints time since program started
  Serial.println(time);
  //    if (int(time) > int(time) + TimeWatering) {
  //      time = millis
  //      myservo.write(150);
  //    }
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
