#include <M5Core2.h>
#include <Wire.h>
#include <WiFi.h>
#include "Ambient.h"
#include "Adafruit_Sensor.h"
#include <Adafruit_BMP280.h>
#include "SHT3X.h"
SHT3X sht30;
Adafruit_BMP280 bme;

#define PERIOD 60

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;

WiFiClient client;
Ambient ambient;


const char* ssid = "********";
const char* password = "*******";

unsigned int channelId = ********; // AmbientのチャネルID
const char* writeKey = "*******"; // ライトキー

void setup() {
  M5.begin();
  Wire.begin();
  M5.Lcd.setBrightness(10);
  M5.Lcd.setTextSize(3);
  Serial.println(F("ENV Unit(SHT30 and BMP280) test..."));

  while (!bme.begin(0x76)){  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  M5.Lcd.clear(BLACK);
  M5.Lcd.println("ENV Unit test...");

  WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }
     Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());
        ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化


}

void loop() {
   int t = millis();
  pressure = bme.readPressure();
  if(sht30.get()==0){
    tmp = sht30.cTemp;
    hum = sht30.humidity;
  }
  //Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%  Pressure: %0.2fPa\r\n", tmp, hum, pressure);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE, BLACK);
  if (pressure > 800.0) { // 電源投入直後に異常値が読めたら捨てる
        M5.Lcd.printf("temp: %2.2f", tmp);
        M5.Lcd.printf(" humid: %0.2f", hum);
        M5.Lcd.printf(" pressure: %f\r\n", pressure);

        // 温度、湿度、気圧、CO2、TVOCの値をAmbientに送信する
        ambient.set(1, String(tmp).c_str());
        ambient.set(2, String(hum).c_str());
        ambient.set(3, String(pressure).c_str());

        ambient.send();
    }

    t = millis() - t;
    t = (t < PERIOD * 1000) ? (PERIOD * 1000 - t) : 1;
    delay(t);

}
