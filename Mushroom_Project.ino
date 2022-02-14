#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define MQPIN 0
#define RLVALUE 5
#define ROCLEANAIRFACTOR 9.83
#define CALIBRATIONTIMES 50
#define CALIBRATIONINTERVAL 500
#define READINTERVAL 50
#define READTIMES 5
#define GAS_CO 1

char auth[] ="Wx2vQ1JmZfHXQhuv1XEZtlcm7rdrKVEx";
const int sensorPin = 4;
int sensorState = 0;
int lastState = 0;
DHT dht(DHTPIN, DHTTYPE);
SimpleTimer timer;
LiquidCrystal_I2C lcd(0x3F, 20, 04);

float COCurve[3] = {2.3, 0.72, -0.34};
float Ro = 10;
int NotificationTemp = 0;
int NotificationHumid = 0;

void sendSensor()
{
  float t = dht.readTemperature();

    if (isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      Blynk.virtualWrite(V6, t);
}

void setup() {
  lcd.begin(20,4);
  lcd.backlight();
  Serial.begin(9600);
  Blynk.begin(auth, "Balotelli", "00000000");
  dht.begin();
  pinMode(sensorPin, INPUT);

  Ro = MQCalibration(MQPIN);
  lcd.setCursor(0,0);
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("** WAHAI MADSUKAA **");
  lcd.setCursor(0,2);
  lcd.print("**    JOM DOTA    **");
  lcd.setCursor(0,4);
  lcd.print("********************");
  
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  lcd.setCursor(0,0);
  lcd.print("**    MUSHROOM    **");
  tempPrint(2,1,t);
  humidPrint(2,2,h);
  coPrint(2,3,GasPercentage(MQread(MQPIN)/Ro));
  notificationApp(NotificationTemp, NotificationHumid);

  delay(1000);
  Blynk.run();
  

}

void notificationApp(int suhu, int humid)
{
  if (suhu > 300){
      Blynk.notify("Suhu kau mencanak njir");
      suhu = 0;
  }

  if(humid > 300){
    Blynk.notify("Woi siram la aku setan");
    humid = 0;
  }
}

void tempPrint(int col, int row, float temp)
{
  lcd.setCursor(col, row);
  lcd.print("TEMP :");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C");
  if(temp > 30.0)
  {
    NotificationTemp += 1;
    
  }
}

void humidPrint(int col, int row, float humid)
{
  lcd.setCursor(col, row);
  lcd.print("HUMID:");
  lcd.print(humid);
  lcd.print("%");
  if(humid < 20.0)
  {
    NotificationHumid += 1;
    
  }
}

void coPrint(int col, int row, int co)
{
  lcd.setCursor(col, row);
  lcd.print("CO2  :");
  lcd.print(co);
  lcd.print("PPM");
}

float MQResistanceCalculation(int raw_adc)
{
  return (((float)RLVALUE*(1023-raw_adc)/raw_adc));
}

float MQCalibration(int mqpin)
{
  int i;
  float val = 0;

  for (i = 0; i < CALIBRATIONTIMES ; i++){
    val += MQResistanceCalculation(analogRead(mqpin));
    delay(CALIBRATIONINTERVAL);
  }

  val = val/CALIBRATIONTIMES;

  val = val/ROCLEANAIRFACTOR;

  return val;
}

float MQread(int mqpin)
{
  int i;
  float rs = 0;

  for (i = 0; i < READTIMES ; i++){
    rs += MQResistanceCalculation(analogRead(mqpin));
    delay(READINTERVAL);
  }

  rs = rs/READTIMES;

  return rs;
}

int GasPercentage(float rs_ro_ratio)
{
  return getPercentage(rs_ro_ratio, COCurve);
}

int getPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,(((log(rs_ro_ratio) - pcurve[1])/pcurve[2]) + pcurve[0])));
}
