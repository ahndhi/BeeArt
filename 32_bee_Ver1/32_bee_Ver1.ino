#include <esp_now.h>
#include <WiFi.h>


// Important
const int myID = 11;

unsigned long lastTime = 0;  
unsigned long timerDelay = 50;  // Send timer
bool enableTX = false;

unsigned long rangeTimer = 0;

uint8_t broadcastAddress1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct bee_struct {
    int id;
} bee_struct;

bee_struct myInfo;
bee_struct rcvInfo;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
}

// Callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&rcvInfo, incomingData, sizeof(rcvInfo));
  int8_t rssi = recv_info->rx_ctrl->rssi;
  if (rcvInfo.id == 99 && rssi > -60) {
    rangeTimer = millis() + 640;
  }
}

 void setup() {
  //Serial.begin(115200);
 
  pinMode(4, INPUT); //Avoid 2, 8, 9
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);  

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    return;
  }

  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  esp_now_register_recv_cb(OnDataRecv);
 
  // register peers
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }  
  delay(1000);
}

void loop() {
  int welcome = digitalRead(4);
  if (welcome == LOW) {
    enableTX = true;
  } else {
    enableTX = false;
  }

  if (millis() < rangeTimer) {
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
  } else {
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }

  if (((millis() - lastTime) > timerDelay) && enableTX == true) {
    myInfo.id = myID;
    esp_now_send(0, (uint8_t *) &myInfo, sizeof(myInfo));
    lastTime = millis();
  }
}