#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "arduino_uno_main.h"
#include "PM2_5.h"

/* PM2.5 sensor globals */
SoftwareSerial pmsSerial(2, 3);
struct pms5003data PMS_data;

/* SD card globals */
File myFile;
char fromESP[DATA_BUFFER];
char timestamp[TIMESTAMP_BUFFER];
char TIME[10];
char DATE[20];

void setup()
{
  /* set reference for ADC */
  analogReference(DEFAULT);

  /* set up LED debuggers */
  pinMode(ERRLED, OUTPUT);
  pinMode(TESTLED, OUTPUT);
  digitalWrite(TESTLED, HIGH);

  /* begin communication with ESP8266 */
  Serial.begin(9600);

  /* wait for ESP8266 to connect to wifi */
  while (1)
  {
    while (!Serial.available())
      ;
    Serial.readBytesUntil('\n', fromESP, DATA_BUFFER);
    if (!strcmp(fromESP, ESP8266_READY))
      break;
  }
  digitalWrite(TESTLED, LOW);

  /* initialize SD card */
  if (!SD.begin(10))
  {
  #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("SD_card_initialization_failed"));
  #endif
    digitalWrite(ERRLED, HIGH);
    while (1)
      ;
  }
  write_files();
  
  /* sensor baud rate is 9600 */
  pmsSerial.begin(9600);
}

void loop()
{
  /* request a timestamp from ESP8266 */
  get_timestamp();

  /* PM2.5 sensor readings available? */
  if (readPMSdata(&pmsSerial, &PMS_data))
  {
    /* reading data was successful! */
    digitalWrite(TESTLED, HIGH);
    write_PMS_to_sd();

    //snprintf(fromESP, DATA_BUFFER, "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", timestamp, PMS_data.pm10_standard, PMS_data.pm25_standard, PMS_data.pm100_standard, PMS_data.pm10_env, PMS_data.pm25_env, PMS_data.pm100_env, PMS_data.particles_03um, PMS_data.particles_05um, PMS_data.particles_10um, PMS_data.particles_25um, PMS_data.particles_50um, PMS_data.particles_100um);
    //write_to_file("pms_data.csv", fromESP, DATA_BUFFER);
  #ifdef MQTT_ON
    publish_PMS_data();
  #endif
    digitalWrite(TESTLED, LOW);
  }

  float CO2_data = get_CO2_measurement();
  if (CO2_data > 0){
    digitalWrite(TESTLED, HIGH);
    write_CO2_to_sd(CO2_data);
    digitalWrite(TESTLED, LOW);
  }
}

/* SD Card Operations */
void write_PMS_to_sd()
{
  myFile = SD.open("pms_data.csv", FILE_WRITE);
  if (myFile)
  {
    char buff[100];
    snprintf(buff, 100, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", PMS_data.pm10_standard, PMS_data.pm25_standard, PMS_data.pm100_standard, PMS_data.pm10_env, PMS_data.pm25_env, PMS_data.pm100_env, PMS_data.particles_03um, PMS_data.particles_05um, PMS_data.particles_10um, PMS_data.particles_25um, PMS_data.particles_50um, PMS_data.particles_100um);
    myFile.write(timestamp);
    myFile.write(buff);
    myFile.close();
    digitalWrite(ERRLED, LOW);
  }
  else
  {
    #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_write_pms_data"));
    #endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  }
}

void write_CO2_to_sd(float ppm)
{
  myFile = SD.open("co2_data.csv", FILE_WRITE);
  if (myFile)
  {
    char buff[20];
    snprintf(buff, 20, ",%s\n", String(ppm).c_str());
    myFile.write(timestamp);
    myFile.write(buff);
    myFile.close();
    digitalWrite(ERRLED, LOW);
  }
  else
  {
    #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_write_pms_data"));
    #endif
    digitalWrite(ERRLED, HIGH);
    digitalWrite(TESTLED, LOW);
  } 
}

void write_to_file(String filename, char *data, int len)
{
  myFile = SD.open(filename, FILE_WRITE);
  if (myFile)
  {
    myFile.write(data);
    myFile.close();
    digitalWrite(ERRLED, LOW);
  }
  else
  {
  #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_write_data_to_file"));
  #endif
    digitalWrite(ERRLED, HIGH);
  }
}

void write_files()
{
  //char title[25];
  //get_timestamp();
  uint8_t f_exists = 0;
  
  /* initialize PMS csv file */
  if (SD.exists("pms_data.csv")) f_exists = 1;

  myFile = SD.open(F("pms_data.csv"), FILE_WRITE);
  if (myFile)
  {
    if (f_exists){
      myFile.print('\n');
      f_exists = 0;
    }
    else {
      myFile.println(F("timestamp,PM_10,PM_25,PM_100,PM_10_env,pm_25_env,pm_100_env,num_particles_gt_03um,num_particles_gt_05um,num_particles_gt_10um,num_particles_gt_25um,num_particles_gt_50um,num_particles_gt_100um"));
    }
    myFile.close();
  }
  else
  {
  #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_open_file_on_SD_card"));
  #endif
    digitalWrite(ERRLED, HIGH);
    while (1)
      ;
  }

  /* initialize CO2 csv file */
  if (SD.exists("co2_data.csv")) f_exists = 1;
  myFile = SD.open(F("co2_data.csv"), FILE_WRITE);
  if (myFile)
  {
    if (f_exists){
      myFile.print('\n');
      f_exists=0;
    }
    else{
      myFile.println(F("timestamp,PPM"));
    }
    myFile.close();
  }
  else
  {
  #ifdef MQTT_ON
    publish_data(String(NETWORK_ID) + String("error_messages"), String("Could_not_open_file_on_SD_card"));
  #endif
    digitalWrite(ERRLED, HIGH);
    while (1)
      ;
  }
}

/* Communication with ESP8266 */
void get_timestamp()
{
  /* request timestamp */
  Serial.print(REQUEST_TIMESTAMP);

  /* First, get date*/
  Serial.readBytesUntil('\n', fromESP, DATA_BUFFER);
  sprintf(DATE, "%s", fromESP);
  memset(fromESP, 0, sizeof(fromESP));

  /* Then, get time */
  Serial.readBytesUntil('\n', fromESP, DATA_BUFFER); 
  sprintf(TIME, "%s", fromESP);

  /* Form timestamp buffer */
  sprintf(timestamp, "%s %s", DATE, TIME);
}

void publish_data(char *data)
{
  Serial.print(data);
  Serial.flush();
}

void publish_PMS_data()
{
  sprintf(fromESP, "%s/PM_10 %d\n", NETWORK_ID, PMS_data.pm10_standard);
  publish_data(fromESP);
  sprintf(fromESP, "%s/PM_25 %d\n", NETWORK_ID, PMS_data.pm25_standard);
  publish_data(fromESP);
  sprintf(fromESP, "%s/PM_100 %d\n", NETWORK_ID, PMS_data.pm100_standard);
  publish_data(fromESP);
  sprintf(fromESP, "%s/PM_10_env %d\n", NETWORK_ID, PMS_data.pm10_env);
  publish_data(fromESP);
  sprintf(fromESP, "%s/PM_25_env %d\n", NETWORK_ID, PMS_data.pm25_env);
  publish_data(fromESP);
  sprintf(fromESP, "%s/PM_100_env %d\n", NETWORK_ID, PMS_data.pm100_env);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_03um %d\n", NETWORK_ID, PMS_data.particles_03um);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_05um %d\n", NETWORK_ID, PMS_data.particles_05um);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_10um %d\n", NETWORK_ID, PMS_data.particles_10um);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_25um %d\n", NETWORK_ID, PMS_data.particles_25um);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_50um %d\n", NETWORK_ID, PMS_data.particles_50um);
  publish_data(fromESP);
  sprintf(fromESP, "%s/particles_100um %d\n", NETWORK_ID, PMS_data.particles_100um);
  publish_data(fromESP);
}

/* Sensor Operations */
float get_CO2_measurement()
{
  /* read ADC value from CO2 sensor */
  int adcVal = analogRead(CO2_PIN);

  /* convert to voltage per CO2 sensor specs */
  float voltage = adcVal * (5000 / 1024.0);
  
  
  if (voltage == 0)
  {
    return -1;
  }
  else if (voltage < 400)
  {
    return -2;
  }
  else
  {
    float voltageDiference = voltage - 400;
    return (float) ((voltageDiference * 50.0) / 16.0);
  }
}