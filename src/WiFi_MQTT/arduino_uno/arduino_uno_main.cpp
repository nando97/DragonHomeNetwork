#include <Arduino.h>
#include "PM2_5.h"

// For PM2.5 sensor 
SoftwareSerial pmsSerial(2, 3);
struct pms5003data data;

void setup() {
  // our debugging output
  Serial.begin(115200);
 
  // sensor baud rate is 9600
  pmsSerial.begin(9600);
}
    
void loop() {
  // PM2.5 sensor readings available?
  if (readPMSdata(&pmsSerial, &data)) {
    // reading data was successful!
    // now, send data to ESP8266 over the serial connection
    publish_data(String(NETWORK_ID) + String("PM_10"), String(data.pm10_standard));
    publish_data(String(NETWORK_ID) + String("PM_25"), String(data.pm25_standard));
    publish_data(String(NETWORK_ID) + String("PM_100"), String(data.pm100_standard));
    publish_data(String(NETWORK_ID) + String("PM_10_env"), String(data.pm10_env));
    publish_data(String(NETWORK_ID) + String("PM_25_env"), String(data.pm25_env));
    publish_data(String(NETWORK_ID) + String("PM_100_env"), String(data.pm100_env));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_03um"), String(data.particles_03um));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_05um"), String(data.particles_05um));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_10um"), String(data.particles_10um));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_25um"), String(data.particles_25um));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_50um"), String(data.particles_50um));
    publish_data(String(NETWORK_ID) + String("num_particles_gt_100um"), String(data.particles_100um));
  }
}

void publish_data(String topic, String data)
{
  Serial.print(topic); Serial.print(" ");
  Serial.print(data); Serial.print("\n");
  Serial.flush();
}