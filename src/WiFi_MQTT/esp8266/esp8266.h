#include <NTPClient.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// these are flipped for esp8266
#define ESP8266_LED_OFF 1
#define ESP8266_LED_ON 0
#define UTC_OFFSET_SECONDS -18000 // offset for Atlantic Time (daylight savings)
#define UPDATE_INTERVAL 360000 // ms, updates every minute
#define MSG_BUFFER_SIZE 100 // bytes, for PubSubClient
#define DATA_BUFFER_SIZE 200

// WiFi settings
#define SSID "wifi-name-goes-here"
#define SSID_PW "wifi-password-goes-here"
#define HOSTNAME "ESP8266-1"

// helper functions
void error_blink_leds(int ms);
void setup_Wifi();
void reconnect_to_broker();
int get_topic_and_topic_msg(String ser_data);
void timestamp_data();