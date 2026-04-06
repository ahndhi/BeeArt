
//#include <ESP8266WiFi.h>
//#include <espnow.h>

#include <esp_now.h>
#include <WiFi.h>
#include <map>
#include <driver/ledc.h>




using namespace std;


unsigned long lastTime = 0;  
unsigned long timerDelay = 3000;
bool noise = false;
int clearCount = 0;
const int inhFreq = 440;


std::map<int, int> rcvBeeData;

// Structure to receive data
// Must match the sender structure
typedef struct bee_struct {
    int id;
} bee_struct;
int rssi;

// Create a bee_struct called beeData to hold the incomming data
bee_struct beeData;

// Callback function that will be executed when data is received
//void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) { 
  memcpy(&beeData, incomingData, sizeof(beeData));
  //rssi = (uint8_t)*(incomingData - 51) - 100; // Pointer offset -51, subtract 100 for dBm
  int8_t rssi = recv_info->rx_ctrl->rssi;
  rcvBeeData[beeData.id] = rssi;
}
 
void setup() {
  // Initialize Serial Monitor
  //Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect(); //NOT IN OTHER VERSION

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }
  //pinMode(0, OUTPUT); //Was 13
  ledcAttach(0, inhFreq, 8);
  pinMode(1, INPUT); //Was 12

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  //esp_now_set_self_role(ESP_NOW_ROLE_SLAVE); //NOT IN OTHER VERSION
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    noise = !noise;
    clearCount = 0;
    lastTime = millis();
  }
  int quiet = digitalRead(1);
  if (noise == true && (rcvBeeData.size() != 0 || quiet == HIGH)) {
    int bigMod = 0;
    for (auto bee : rcvBeeData) {
      if (bee.first == 11) {
        float mod = ((880.0 - inhFreq) * ((100.0 - abs(bee.second)) / 100.0));
        bigMod += mod;
      }
      if (bee.first == 22) {
        float mod = ((220.0 - inhFreq) * ((100.0 - abs(bee.second)) / 100.0));
        bigMod += mod;
      }
    }
    int outFreq = inhFreq + bigMod;
    //analogWriteFrequency(0,outFreq);
    ledc_set_freq(LEDC_LOW_SPEED_MODE,LEDC_TIMER_0,outFreq);
    //analogWrite(0,127);
    ledcWrite(0, 127);
  } else {
    rcvBeeData.clear();
    //analogWrite(0,0);
    ledcWrite(0, 0);
  }

}
