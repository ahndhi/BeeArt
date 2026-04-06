#include <esp_now.h>
#include <WiFi.h>

//#include <ESP8266WiFi.h>
//#include <espnow.h>

int myID = 0;
int sendCnt = 0;
// REPLACE WITH RECEIVER MAC Address

// TO DO!!!!!!
uint8_t broadcastAddress1[] = {0x1C, 0xDB, 0xD4, 0x3B, 0x20, 0x0C};
uint8_t broadcastAddress2[] = {0x1C, 0xDB, 0xD4, 0x3B, 0x39, 0x90};
uint8_t broadcastAddress3[] = {0x88, 0x56, 0xA6, 0x6F, 0xF4, 0x58};


// Structure example to send data
// Must match the receiver structure
typedef struct bee_struct {
    int id;
} bee_struct;

// Create a struct_message called test to store variables to be sent
bee_struct myInfo;

esp_now_peer_info_t peerInfo; //Necessary to add the Flowers to the send list, I think.

unsigned long lastTime = 0;  
unsigned long timerDelay = 50;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // char macStr[18];
  // Serial.print("Packet to:");
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print(macStr);
  // Serial.print(" send status: ");
  // if (sendStatus == 0){
  //   Serial.println("Delivery success");
  // }
  // else{
  //   Serial.println("Delivery fail");
  // }
}
 
void setup() {
  // Init Serial Monitor
  //Serial.begin(115200);
  pinMode(1, INPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect(); //DOES NOT EXIST IN 32 Ver.

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    //Serial.println("Error initializing ESP-NOW");
    return;
  }

  //esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); 
  
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  //esp_now_register_send_cb(OnDataSent);
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  
  // Register peer
//  esp_now_add_peer(broadcastAddress1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
//  esp_now_add_peer(broadcastAddress2, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // register peer1
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }

  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }

  // register third peer  
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }


}
 
void loop() {
  int toSend = digitalRead(1);
  if (toSend == HIGH && sendCnt < 1){
    myID += 1;
    sendCnt = 50;
  }
  if ((millis() - lastTime) > timerDelay && sendCnt > 0) {
    // Set values to send
    myInfo.id = myID;

    // Send message via ESP-NOW
    esp_now_send(0, (uint8_t *) &myInfo, sizeof(myInfo));
    sendCnt -= 1;
    lastTime = millis();
  }
}
