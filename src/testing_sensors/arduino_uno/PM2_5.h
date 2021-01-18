#include <SoftwareSerial.h>
#include <Arduino.h>
#define NETWORK_ID "DHN/0/0/"

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

boolean readPMSdata(Stream *s, pms5003data *data);
void publish_data(String topic, String data);