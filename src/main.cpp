#include <FastLED.h>
#include <esp_now.h>
#include <WiFi.h>

#define NUM_LEDS 8
#define DATA_PIN 23
#define CHANNEL 1

CRGB leds[NUM_LEDS];
uint8_t transitionTime = 0;

// Callback function for receiving ESP-NOW messages
void onDataReceived(const uint8_t *macAddr, const uint8_t *data, int len) {
  // Check that the data length is correct
  if (len != NUM_LEDS * 4) {
    Serial.println("Invalid message length");
    return;
  }

  // Extract the RGB and speed values from the data
  for (int i = 0; i < NUM_LEDS; i++) {
    int offset = i * 4;
    leds[i] = CRGB(data[offset], data[offset+1], data[offset+2]);
    transitionTime = data[offset+3];
  }

  // Update the LED strip
  FastLED.show();
}

void setup() {
  // Initialize FastLED library
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_err_t result = esp_now_init();
  if (result != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback function for receiving messages
  esp_now_register_recv_cb(onDataReceived);

  // Add the broadcast address to the ESP-NOW peer list
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  peerInfo.channel = CHANNEL;
  peerInfo.ifidx = ESP_IF_WIFI_STA;
  peerInfo.encrypt = false;
  memcpy(peerInfo.peer_addr, (uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 6);
  esp_now_add_peer(&peerInfo);

  Serial.println("Setup complete");
}

void loop() {
  // Set the transition time for the next update
  uint16_t transitionDuration = (uint16_t) (transitionTime / 255.0 * 500); // 500ms is the default transition time
  FastLED.setDither(0); // disable dithering for consistent timing
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setTemperature(Tungsten100W);
  FastLED.setBrightness(255);
  FastLED.show();
  FastLED.delay(transitionDuration);

  // Put the ESP32 into deep sleep mode to conserve power
  esp_deep_sleep_start();
}
