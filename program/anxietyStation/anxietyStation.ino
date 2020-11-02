#include <LiquidCrystal.h>

/* Configuracion leds*/
int ledOnOFf = 48;
int ledGreen_AirSensor = 30;
int ledYellow_AirSensor = 32;
int ledRed_AirSensor = 34;
int ledGreen_PhotoSensor = 36;
int ledYellow_PhotoSensor = 38;
int ledRed_PhotoSensor = 40;

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

  // lcd parameters -> columns, rows
  lcd.begin(16, 2);
}

void loop()
{
  Serial.begin(bps);
  digitalWrite(ledOnOFf, HIGH);

  AirSensorControl();
  PhotoSensorControl();

  showData();

  delay(aplicationDelay);
}

void AirSensorControl()
{
  state_AirSensor = digitalRead(airSensorDig);
  ppm_AirSensor = analogRead(airSensorAnalog);
  tension_AirSensor = ppm_AirSensor * (5.0 / 1023.0);

  resultAirSensorPPM = "AirSensor current ppm: ";
  resultAirSensorPPM.concat(ppm_AirSensor);

  if (!state_AirSensor || ppm_AirSensor >= 700)
  {
    digitalWrite(ledGreen_AirSensor, LOW);
    digitalWrite(ledYellow_AirSensor, LOW);
    digitalWrite(ledRed_AirSensor, HIGH);

    resultAirSensorPPM.concat(" - Danger");
  }
  else if (ppm_AirSensor >= 100 && ppm_AirSensor < 700)
  {
    digitalWrite(ledGreen_AirSensor, LOW);
    digitalWrite(ledYellow_AirSensor, HIGH);
    digitalWrite(ledRed_AirSensor, LOW);

    resultAirSensorPPM.concat(" - Warning");
  }
  else if (ppm_AirSensor > 0 && ppm_AirSensor < 100)
  {
    digitalWrite(ledGreen_AirSensor, HIGH);
    digitalWrite(ledYellow_AirSensor, LOW);
    digitalWrite(ledRed_AirSensor, LOW);

    resultAirSensorPPM.concat(" - OK");
  }
  else
  {
    digitalWrite(ledGreen_AirSensor, HIGH);
    digitalWrite(ledYellow_AirSensor, HIGH);
    digitalWrite(ledRed_AirSensor, HIGH);

    resultAirSensorPPM.concat(" - ERROR");
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
  
  resultPhotoSensor = "PhotoSensor value: ";
  resultPhotoSensor.concat(photoVoltage);
}

String moveLeftDisplay(String stringToShow)
{
  String result;
  String StrProcess = "                " + stringToShow + "                ";
  result = StrProcess.substring(InitialPositionMoveDisplay, PositionsToMoveDisplay);

  InitialPositionMoveDisplay++;
  PositionsToMoveDisplay++;

  if (InitialPositionMoveDisplay > StrProcess.length())
  {
    InitialPositionMoveDisplay = 16;
    PositionsToMoveDisplay = 0;
  }

  return result;
}

void showData()
{
  String dataToShow = resultAirSensorPPM + ", " + resultPhotoSensor;

  lcd.clear();
  writeDataInLcd(0, 0, "Anxiety Station");
  writeDataInLcd(0, 1, moveLeftDisplay(dataToShow));
}

void writeDataInLcd(int columnPosition, int rowPosition, String textToWrite)
{
  lcd.setCursor(columnPosition, rowPosition);
  lcd.print(textToWrite);
}
