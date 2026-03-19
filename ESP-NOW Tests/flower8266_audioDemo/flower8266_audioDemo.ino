/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp8266-nodemcu/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <map>

using namespace std;


unsigned long lastTime = 0;  
unsigned long timerDelay = 3000;
bool noise = false;
int clearCount = 0;
const int inhFreq = 440;


std::map<int, int> rcvBeeData;

// Structure example to receive data
// Must match the sender structure
typedef struct bee_struct {
    int id;
} bee_struct;
int rssi;

// Create a struct_message called myData
bee_struct beeData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&beeData, incomingData, sizeof(beeData));
  rssi = (uint8_t)*(incomingData - 51) - 100; // Pointer offset -51, subtract 100 for dBm

  rcvBeeData[beeData.id] = rssi;
}
 
void setup() {
  // Initialize Serial Monitor
  //Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  pinMode(13, OUTPUT);
  pinMode(12, INPUT);

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    noise = !noise;
    clearCount = 0;
    lastTime = millis();
  }
  int quiet = digitalRead(12);
  if (noise == true && (rcvBeeData.size() != 0 || quiet == HIGH)) {
    int bigMod = 0;
    for (auto bee : rcvBeeData) {
      if (bee.first == 11) {
        float mod = ((880.0 - inhFreq) * ((100.0 - abs(bee.second)) / 100.0));
        bigMod += mod;
      }
      if (bee.first == 22) {
        float mod = ((120.0 - inhFreq) * ((100.0 - abs(bee.second)) / 100.0));
        bigMod += mod;
      }
    }
    int outFreq = inhFreq + bigMod;
    analogWriteFreq(outFreq);
    analogWrite(13,127);
  } else {
    rcvBeeData.clear();
    analogWrite(13,0);
  }

}