
/*
   We use SparkFun code, firmware, and software that is released under the MIT License(http://opensource.org/licenses/MIT).
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

/* Configuracion leds*/
int ledOnOFf = 48;
int ledGreen_AirSensor = 30;
int ledYellow_AirSensor = 32;
int ledRed_AirSensor = 34;
int ledGreen_PhotoSensor = 36;
int ledYellow_PhotoSensor = 38;
int ledRed_PhotoSensor = 40;
int ledGreen_SPO2Sensor = 42;
int ledYellow_SPO2Sensor = 44;
int ledRed_SPO2Sensor = 46;

/* Configuracion Sensores */
int airSensorDig = 10;
int airSensorAnalog = 3;
int ppm_AirSensor = 0;
float tension_AirSensor = 0;
bool state_AirSensor = false;
String resultAirSensorPPM = "";

int photoSensorDig = 9;
int photoSensorAnalog = 0;
String resultPhotoSensor = "";

MAX30105 heartAndOxigenSensor;
uint32_t irBuffer[50];  //infrared LED sensor data
uint32_t redBuffer[50]; //red LED sensor data
int32_t bufferLength;   //data length
int32_t spo2;           //SPO2 value
int8_t validSPO2;       //indicator to show if the SPO2 calculation is valid
int32_t heartRate;      //heart rate value
int8_t validHeartRate;  //indicator to show if the heart rate calculation is valid
bool isFirstIteration;
String resultHearAndOxigenSensor = "";

/* Configuracion LCD */
const int rs = 8, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int InitialPositionMoveDisplay = 16;
int PositionsToMoveDisplay = 0;

/* Otras variables*/
long bps = 9600;
int aplicationDelay = 500;

void setup()
{
  // Led onoff
  pinMode(ledOnOFf, OUTPUT);

  // Sensors
  pinMode(ledGreen_AirSensor, OUTPUT);
  pinMode(ledYellow_AirSensor, OUTPUT);
  pinMode(ledRed_AirSensor, OUTPUT);
  pinMode(ledGreen_PhotoSensor, OUTPUT);
  pinMode(ledYellow_PhotoSensor, OUTPUT);
  pinMode(ledRed_PhotoSensor, OUTPUT);
  pinMode(ledGreen_SPO2Sensor, OUTPUT);
  pinMode(ledYellow_SPO2Sensor, OUTPUT);
  pinMode(ledRed_SPO2Sensor, OUTPUT);

  isFirstIteration = true;
  bufferLength = 50; //buffer length of 100 stores 4 seconds of samples running at 25sps

  // Initialize sensor
  if (!heartAndOxigenSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    while (1)
    {
    }
  }

  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4;  //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;        //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 50;    //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;    //Options: 69, 118, 215, 411
  int adcRange = 4096;     //Options: 2048, 4096, 8192, 16384

  heartAndOxigenSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  // lcd parameters -> columns, rows
  lcd.begin(16, 2);
}

void loop()
{
  digitalWrite(ledOnOFf, HIGH);

  AirSensorControl();
  PhotoSensorControl();
  HeartAndOxigenSensorControl();

  showData();

  delay(aplicationDelay);
}

void AirSensorControl()
{
  state_AirSensor = digitalRead(airSensorDig);
  ppm_AirSensor = analogRead(airSensorAnalog);
  tension_AirSensor = ppm_AirSensor * (5.0 / 1023.0);

  resultAirSensorPPM = "";
  resultAirSensorPPM.concat(ppm_AirSensor);

  if (!state_AirSensor || ppm_AirSensor >= 700)
  {
    digitalWrite(ledGreen_AirSensor, LOW);
    digitalWrite(ledYellow_AirSensor, LOW);
    digitalWrite(ledRed_AirSensor, HIGH);
  }
  else if (ppm_AirSensor >= 100 && ppm_AirSensor < 700)
  {
    digitalWrite(ledGreen_AirSensor, LOW);
    digitalWrite(ledYellow_AirSensor, HIGH);
    digitalWrite(ledRed_AirSensor, LOW);
  }
  else if (ppm_AirSensor > 0 && ppm_AirSensor < 100)
  {
    digitalWrite(ledGreen_AirSensor, HIGH);
    digitalWrite(ledYellow_AirSensor, LOW);
    digitalWrite(ledRed_AirSensor, LOW);
  }
  else
  {
    digitalWrite(ledGreen_AirSensor, HIGH);
    digitalWrite(ledYellow_AirSensor, HIGH);
    digitalWrite(ledRed_AirSensor, HIGH);
  }
}

void PhotoSensorControl()
{
  int sensorValue = analogRead(photoSensorAnalog);
  float photoVoltage = sensorValue * (5.0 / 1024.0);

  if (photoVoltage >= 0 && photoVoltage <= 1)
  {
    digitalWrite(ledGreen_PhotoSensor, HIGH);
    digitalWrite(ledYellow_PhotoSensor, LOW);
    digitalWrite(ledRed_PhotoSensor, LOW);
  }
  else if (photoVoltage > 1 && photoVoltage <= 2)
  {
    digitalWrite(ledGreen_PhotoSensor, LOW);
    digitalWrite(ledYellow_PhotoSensor, HIGH);
    digitalWrite(ledRed_PhotoSensor, LOW);
  }
  else if (photoVoltage > 2)
  {
    digitalWrite(ledGreen_PhotoSensor, LOW);
    digitalWrite(ledYellow_PhotoSensor, LOW);
    digitalWrite(ledRed_PhotoSensor, HIGH);
  }
  else
  {
    digitalWrite(ledGreen_PhotoSensor, HIGH);
    digitalWrite(ledYellow_PhotoSensor, HIGH);
    digitalWrite(ledRed_PhotoSensor, HIGH);
  }

  resultPhotoSensor = "";
  resultPhotoSensor.concat(photoVoltage);
}

void HeartAndOxigenSensorControl()
{
  if (isFirstIteration)
  {
    isFirstIteration = false;

    //read the first bufferLength samples, and determine the signal range
    for (byte i = 0; i < bufferLength; i++)
    {
      while (heartAndOxigenSensor.available() == false) //do we have new data?
      {
        heartAndOxigenSensor.check(); //Check the sensor for new data
      }

      redBuffer[i] = heartAndOxigenSensor.getRed();
      irBuffer[i] = heartAndOxigenSensor.getIR();
      heartAndOxigenSensor.nextSample(); //We're finished with this sample so move to next sample
    }

    //calculate heart rate and SpO2 after first 50 samples (first 4 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }

  //dumping the first 12 sets of samples in the memory and shift the last 50 sets of samples to the top
  for (byte i = 12; i < bufferLength; i++)
  {
    redBuffer[i - 12] = redBuffer[i];
    irBuffer[i - 12] = irBuffer[i];
  }

  //take 38 sets of samples before calculating the heart rate.
  for (byte i = 38; i < bufferLength; i++)
  {
    while (heartAndOxigenSensor.available() == false) //do we have new data?
    {
      heartAndOxigenSensor.check(); //Check the sensor for new data
    }

    redBuffer[i] = heartAndOxigenSensor.getRed();
    irBuffer[i] = heartAndOxigenSensor.getIR();

    heartAndOxigenSensor.nextSample(); //We're finished with this sample so move to next sample
  }

  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  if (validSPO2 == 1)
  {
    resultHearAndOxigenSensor = "";
    resultHearAndOxigenSensor.concat(spo2);

    if (spo2 > 90)
    {
      digitalWrite(ledGreen_SPO2Sensor, HIGH);
      digitalWrite(ledYellow_SPO2Sensor, LOW);
      digitalWrite(ledRed_SPO2Sensor, LOW);
    }
    else if (spo2 > 70 && spo2 <= 90)
    {
      digitalWrite(ledGreen_SPO2Sensor, LOW);
      digitalWrite(ledYellow_SPO2Sensor, HIGH);
      digitalWrite(ledRed_SPO2Sensor, LOW);
    }
    else if (spo2 > 0)
    {
      digitalWrite(ledGreen_SPO2Sensor, LOW);
      digitalWrite(ledYellow_SPO2Sensor, LOW);
      digitalWrite(ledRed_SPO2Sensor, HIGH);
    }
    else
    {
      digitalWrite(ledGreen_SPO2Sensor, HIGH);
      digitalWrite(ledYellow_SPO2Sensor, HIGH);
      digitalWrite(ledRed_SPO2Sensor, HIGH);
    }
  }
  else
  {
    resultHearAndOxigenSensor = "";
    resultHearAndOxigenSensor.concat(" - ");
  }
}

void showData()
{
  String dataToShow = resultAirSensorPPM + "   " + resultPhotoSensor + "   " + resultHearAndOxigenSensor;

  lcd.clear();
  writeDataInLcd(0, 0, "PPM  LIGHT  SPO2");
  writeDataInLcd(0, 1, dataToShow);
}

void writeDataInLcd(int columnPosition, int rowPosition, String textToWrite)
{
  lcd.setCursor(columnPosition, rowPosition);
  lcd.print(textToWrite);
}
