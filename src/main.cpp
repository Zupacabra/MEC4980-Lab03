#define MICRO
// #define NARROW
// #define TRANSPARENT

//////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Arduino.h>

#include <stdint.h>

// Include the SparkFun qwiic OLED Library
#include <SparkFun_Qwiic_OLED.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

#if defined(TRANSPARENT)
QwiicTransparentOLED myOLED;
const char *deviceName = "Transparent OLED";

#elif defined(NARROW)
QwiicNarrowOLED myOLED;
const char *deviceName = "Narrow OLED";

#else
QwiicMicroOLED myOLED;
const char *deviceName = "Micro OLED";

#endif

int yoffset;
int LEDpin = 10;
float targetTemp = 20.0;
float temp = 25.0;
const char *degreeSys[] = {"C", "F", "K", "R"};
int currentSystem = 0; // 0 = C, 1 = F, 2 = K, 3 = R
float scaledTargetTemp = targetTemp;
float scaledTemp = 0.0;
int pinButton1 = 5;
int pinButton2 = 6;
int pinButton3 = 9;
bool prevPressed1 = false;
bool prevPressed2 = false;
bool prevPressed3 = false;

enum MachineState
{
  DisplayTemps,
  SetTemp,
  ChooseSystem
};
MachineState currentState;

void setup()
{
  Serial.begin(9600);
  currentState = DisplayTemps;
  pinMode(pinButton1, INPUT_PULLDOWN);
  pinMode(pinButton2, INPUT_PULLDOWN);
  pinMode(pinButton3, INPUT_PULLDOWN);
  pinMode(LEDpin, OUTPUT);

  delay(3000);

  if (!bme.begin(0x77, &Wire))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1)
      ;
  }

  delay(500); // Give display time to power on

  Serial.println("\n\r-----------------------------------");

  Serial.print("Running Test #5 on: ");
  Serial.println(String(deviceName));

  if (!myOLED.begin())
  {
    Serial.println("- Device Begin Failed");
    while (1)
      ;
  }

  yoffset = (myOLED.getHeight() - myOLED.getFont()->height) / 2;

  delay(1000);
}

void loop()
{
  if (digitalRead(pinButton1) == HIGH && !prevPressed1)
  {
    currentState = MachineState(((int)currentState + 1) % 3);
  }
  prevPressed1 = digitalRead(pinButton1);
  char myNewText[50];

  if (currentState == DisplayTemps)
  {
    temp = bme.readTemperature();

    switch (currentSystem)
    {
    default: // Celsius
      scaledTemp = temp;
      break;
    case 1: // Fahrenheit
      scaledTemp = round(((temp * 9.0 / 5.0) + 32.0)*10)/10.0;
      break;
    case 2: // Kelvin
      scaledTemp = round((temp + 273.15)*10)/10.0;
      break;
    case 3: // Rankine
      scaledTemp = round(((temp + 273.15) * 9.0 / 5.0)*10)/10.0;
      break;
    }

    sprintf(myNewText, "T: %.1f %s", scaledTemp, degreeSys[currentSystem]); // %s = character/string, %d = integer, %f = float

    myOLED.erase();
    myOLED.text(3, yoffset - 12, myNewText);

    sprintf(myNewText, "Ttar: %.1f %s", scaledTargetTemp, degreeSys[currentSystem]);
    myOLED.text(3, yoffset + 12, myNewText);
  }

  else if (currentState == SetTemp)
  {
    if (digitalRead(pinButton2) == HIGH && !prevPressed2)
    {
      targetTemp += 0.5;
    }
    prevPressed2 = digitalRead(pinButton2);
    if (digitalRead(pinButton3) == HIGH && !prevPressed3)
    {
      targetTemp -= 0.5;
    }
    prevPressed3 = digitalRead(pinButton3);

    if (prevPressed2 || prevPressed3)
    {
      switch (currentSystem)
      {
      default: // Celsius
        scaledTargetTemp = targetTemp;
        break;
      case 1: // Fahrenheit
        scaledTargetTemp = round(((targetTemp * 9.0 / 5.0) + 32.0)*10)/10.0;
        break;
      case 2: // Kelvin
        scaledTargetTemp = round((targetTemp + 273.15)*10)/10.0;
        break;
      case 3: // Rankine
        scaledTargetTemp = round(((targetTemp + 273.15) * 9.0 / 5.0)*10)/10.0;
        break;
      }
    }

    sprintf(myNewText, "Ttar: %.1f %s", scaledTargetTemp, degreeSys[currentSystem]);
    myOLED.erase();
    myOLED.text(3, yoffset, myNewText);
  }

  else if (currentState == ChooseSystem)
  {
    if (digitalRead(pinButton2) == HIGH && !prevPressed2)
    {
      currentSystem = (currentSystem + 1) % 4;
    }
    prevPressed2 = digitalRead(pinButton2);
    if (digitalRead(pinButton3) == HIGH && !prevPressed3)
    {
      currentSystem = abs((currentSystem - 1) % 4);
    }
    prevPressed3 = digitalRead(pinButton3);

    if (prevPressed2 || prevPressed3)
    {
      switch (currentSystem)
      {
      default: // Celsius
        scaledTargetTemp = targetTemp;
        break;
      case 1: // Fahrenheit
        scaledTargetTemp = round(((targetTemp * 9.0 / 5.0) + 32.0)*10)/10.0;
        break;
      case 2: // Kelvin
        scaledTargetTemp = round((targetTemp + 273.15)*10)/10.0;
        break;
      case 3: // Rankine
        scaledTargetTemp = round(((targetTemp + 273.15) * 9.0 / 5.0)*10)/10.0;
        break;
      }
    }

    sprintf(myNewText, "System: %s", degreeSys[currentSystem]);
    myOLED.erase();
    myOLED.text(3, yoffset, myNewText);
  }
  
  if (temp < targetTemp){
    digitalWrite(LEDpin, 1);
  }else{
    digitalWrite(LEDpin, 0);
  }

  myOLED.display();
  delay(25);
}