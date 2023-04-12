#include <stdint.h>

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define ESP_NOW_CHANNEL 3
#define SLEEP_TIMEOUT 60 // seconds
#define NUM_LEDS 8
uint8_t BroadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};