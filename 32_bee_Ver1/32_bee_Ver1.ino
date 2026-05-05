#include <esp_now.h>
#include <WiFi.h>
#include "AudioTools.h"
#include "sam_arduino.h"

unsigned long lastTime = 0;  
unsigned long timerDelay = 50;  // Send timer
bool enableTX = false;

bool inRange = false;
unsigned long rangeLastTime = 0;
unsigned long rangeTimer = 10000;
int failCount = 0;
int noteShot = 500;
bool welcomeRst = false;

// Important
const int myID = 11;

// TO DO!!!!!!
uint8_t broadcastAddress1[] = {0x1C, 0xDB, 0xD4, 0x3B, 0x20, 0x0C};

typedef struct bee_struct {
    int id;
} bee_struct;

bee_struct myInfo;

esp_now_peer_info_t peerInfo;

AudioInfo info(8000, 1, 16);
PWMAudioOutput pwm;
SAM sam(pwm);

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    inRange = true;
    timerDelay = 50;
  }
  else{
    failCount += 1;
  }
}
 
void setup() {
  //Serial.begin(115200);

  pinMode(1, INPUT); //Avoid 2, 8, 9
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    return;
  }
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

  // register peers
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }

  sam.setVoice(SAM::Sam);
  // setup PWM output
  auto config = pwm.defaultConfig();
  config.copyFrom(info);
  config.sample_rate = 22050;
  config.bits_per_sample = 16;
  config.resolution = 8;  // must be between 8 and 11 -> drives pwm frequency (8 is default)
  config.pwm_frequency = 40000;
  config.start_pin = 0;
  pwm.begin(config);

  delay(2500);
}
 
void loop() {
  int welcome = digitalRead(1);
  if (welcome == HIGH) {
    enableTX = true;
    if (welcomeRst == true) {
      welcomeRst = false;
      timerDelay = 500;
      sam.say("Welcome Garden Explorer!");
    }
  }
  if (welcome == LOW) {
    enableTX = false;
    welcomeRst = true;
  }

  if ((millis() - rangeLastTime) > rangeTimer) {
    if (failCount >= 150) {
      inRange = false;
      timerDelay = 500;
    }
    if (inRange == true) {
      noteShot = 0;
      inRange = false;
    }
    rangeLastTime = millis();
    failCount = 0;
  }

  if (noteShot < 500) {
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    noteShot += 1;
  }
  if (noteShot >= 500) {
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }

  if (((millis() - lastTime) > timerDelay) && enableTX == true) {
    myInfo.id = myID;
    esp_now_send(0, (uint8_t *) &myInfo, sizeof(myInfo));
    lastTime = millis();
    //Serial.println("???");
  }
}
