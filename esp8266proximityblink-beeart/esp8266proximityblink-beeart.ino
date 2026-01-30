#include "ESP8266WiFi.h"


long lastBlinkMillis;
boolean ledState;

#define SCAN_PERIOD 5000
long lastScanMillis;
bool toScan = true;
int blink = 5000;


void setup()
 {
  Serial.begin(115200);
  Serial.println();


  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(13, OUTPUT);

  WiFi.disconnect();
  WiFi.softAP("Flower");
  delay(100);
}

void loop()
{
  long currentMillis = millis();

  // blink LED
  if (currentMillis - lastBlinkMillis > blink)
  {
    digitalWrite(LED_BUILTIN, ledState);
    ledState = !ledState;
    lastBlinkMillis = currentMillis;
  }

  // trigger Wi-Fi network scan
  if (currentMillis - lastScanMillis > SCAN_PERIOD && toScan == true)
  {
    WiFi.scanNetworks(true);
    lastScanMillis = currentMillis;
  }
  digitalWrite(13, LOW);
  // print out Wi-Fi network scan result upon completion
  int n = WiFi.scanComplete();
  bool found = false;
  if(n >= 0)
  {
    digitalWrite(13, HIGH);
    for (int i = 0; i < n; i++)
    {
      if(WiFi.SSID(i) == "Flower")
      {
        WiFi.begin("Flower");
        toScan = false;
      } 
    }
    WiFi.scanDelete();
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    toScan = false;
    blink = 1250;
    if(WiFi.RSSI() > -80 && WiFi.RSSI() <= -70) {
      blink = 750;
    } else if(WiFi.RSSI() > -70 && WiFi.RSSI() <= -60) {
      blink = 500;
    } else if(WiFi.RSSI() > -60 && WiFi.RSSI() <= -50) {
      blink = 250;
    } else if(WiFi.RSSI() > -40 && WiFi.RSSI() <= -30) {
      blink = 150;
    } else if(WiFi.RSSI() > -30) {
      blink = 50;
    }
  } else {
    toScan = true;
    blink = 5000;
  }
}