#include <WiFi.h>
#include <esp_wifi.h>

uint8_t mac[6];

void setup(){
  Serial.begin(115200);

  // initalize Wi-Fi to be able to read MAC address
  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();

  // read base address
  esp_wifi_get_mac(WIFI_IF_STA, mac);
}
 
void loop(){
  delay(1000);
  Serial.printf("0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                mac[0], mac[1], mac[2],
                mac[3], mac[4], mac[5]);
}