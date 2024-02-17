#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <FastLED.h>
#include "config.h"


#define NUM_LEDS 8
#define DATA_PIN 4

#define CHANNEL 1
#define MAX_BRIGHTNESS 255

CRGB leds[NUM_LEDS];

// Data structure to hold received message
struct Message {
  uint8_t timeout; // This is the number of seconds to wait before entering deep sleep
  struct LEDState {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t speed;
  } leds[NUM_LEDS];
};

// Callback function that will be called when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // Cast incoming data to message struct
  Message message;
  memcpy(&message, incomingData, sizeof(message));

  // Update LED colors and transitions based on the new message
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(message.leds[i].red, message.leds[i].green, message.leds[i].blue);
    uint8_t speed = message.leds[i].speed;
    if (speed == 0) {
      leds[i].fadeToBlackBy(0);
    } else {
      uint16_t transitionTime = map(speed, 0, 255, 0, 500);
      leds[i].fadeToBlackBy(transitionTime);
    }
  }

  // Show updated LED colors
  FastLED.show();
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize FastLED library
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
  leds[1].setRGB(0, 255, 0)

  // Set device as a Wi-Fi station
  WiFi.mode(WIFI_STA);

  // Set the devies mac address
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  // Set peer devices for ESP-NOW communication
  esp_now_peer_info_t peer;
  memset(&peer, 0, sizeof(peer));
  memcpy(peer.peer_addr, BroadcastAddress, 6);
  peer.channel = ESP_NOW_CHANNEL;
  peer.ifidx = WIFI_IF_STA;
  peer.encrypt = false;
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Error setting up ESP-NOW peer");
    return;
  }
}

void loop() {
  // Do nothing in the loop, data is received in the callback function
}
