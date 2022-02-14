#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>


////INITIALIZATION////
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial GSM(7, 8);
int count = 0;
int pinsoilSensor = A0;
int soilSensor = 0;
int relayPump = 10;
bool x = true;
unsigned char buffer[64];

void setup() {
    pinMode(pinsoilSensor, INPUT);
    pinMode(relayPump, OUTPUT);

    GSM.begin(9600);
    Serial.begin(9600);
    lcd.begin();
    lcd.setCursor(12, 1);
    lcd.print("OFF ");
  }

void loop() {
    soilSensor = digitalRead(pinsoilSensor);

    lcd.setCursor(0, 0);
    lcd.print("Soil Moist :");
    lcd.setCursor(0, 1);
    lcd.print("Pump Stat. :");

    if ((soilSensor == HIGH)&& (x == true)) {
        lcd.setCursor(12, 1);
        lcd.print("ON");
        lcd.setCursor(12, 0);
        lcd.print("LOW");
        hantarMesej("Soil moisture LOW, Pump ON");
        digitalWrite(relayPump, HIGH);
        delay(1000);
        x = false;
      }
     else if ((soilSensor == LOW) && (x == false)){
        lcd.setCursor(12, 1);
        lcd.print("OFF");
        lcd.setCursor(12, 0);
        lcd.print("HIGH");
        hantarMesej("Soil moisture HIGH. Pump OFF");
        digitalWrite(relayPump, LOW);
        delay(1000);
      }
  }

void hantarMesej(String mesej) {
    GSM.println("AT+CMGF=1");
    delay(1000);
    GSM.println("AT+CMGS=\"+60102474542\"\r");
    delay(1000);
    GSM.println(mesej);
    delay(100);
    GSM.println((char)26);
    delay(1000);
  }
