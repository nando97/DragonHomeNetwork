#include "NTPClient.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "esp8266.h"

// WiFi objects
IPAddress staticIP(192, 168, 0, 68);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
IPAddress MQTTbroker(192, 168, 0, 189);

// For timestamp
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char GLOBAL_TIMESTAMP[60];
char DATE[10];
char TIME[15];

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_SECONDS, UPDATE_INTERVAL);

// Set up MQTT client
WiFiClient espClient;
PubSubClient MQTTclient(espClient);

// Topic variables
char TOPIC_MSG[MSG_BUFFER_SIZE];
char *TOPIC;
char *TOPIC_DATA;


void setup()
{
    pinMode(ERR_LED, OUTPUT); /* configure LED on ESP8266 for debugging use */
    digitalWrite(ERR_LED, HIGH);
    Serial.begin(9600);     /* begin serial monitoring */
#ifdef DEBUG
    Serial.println("started serial monitor");
#endif
    setup_Wifi(); /* configure wifi settings */
    timeClient.begin(); /* begin the NTP Client */
    MQTTclient.setServer(MQTTbroker, 1883); /* start the MQTT client */
    //MQTTclient.setCallback(callback); /* function to handle subscription updates */
    digitalWrite(ERR_LED, LOW);
    Serial.print("done!\n");
}

void loop()
{
    timeClient.update(); /* updates the time based on an interval (see .h file for settings) */
    if (!MQTTclient.connected()) /* connection broken? */
         reconnect_to_broker();
    MQTTclient.loop();
    if (Serial.available() > 0) /* is data is available from Arduino? */
    {
        char s_buff[100];
        int bytes_read = Serial.readBytesUntil('\n', s_buff, 99); /* read arduino data from serial port */
        s_buff[bytes_read] = '\0';
        if (!strcmp(s_buff, TIMESTAMP_REQUEST)){
            set_timestamp();
            print_timestamp();
        }
        else if (get_topic_and_topic_msg(s_buff)) /* topic name and data legible? */
        {
            MQTTclient.publish(TOPIC, TOPIC_MSG);  /* publish to topic */
        }
    }
    // digitalWrite(ERR_LED, HIGH);
    // set_timestamp();
    // print_timestamp();
    // digitalWrite(ERR_LED, LOW);
    // delay(1000);
}

void setup_Wifi()
{
    delay(3000);
    digitalWrite(ERR_LED, HIGH);
    WiFi.hostname(HOSTNAME);
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS))
    {
#ifdef DEBUG
        Serial.println("STA failed to configure");
#endif
        error_blink_leds(200);
    }

#ifdef DEBUG
    Serial.print("Connecting...");
#endif
    WiFi.begin(SSID, SSID_PW);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
#ifdef DEBUG
        Serial.print(".");
#endif
    }
    digitalWrite(ERR_LED, LOW);
#ifdef DEBUG
    Serial.print("Connected!\nIP Address: ");
    Serial.println(WiFi.localIP());
#endif
}

/* function used to reconnect to MQTT broker in case of a lost cnxn */
void reconnect_to_broker()
{
    // Loop until we're reconnected
    while (!MQTTclient.connected())
    {
// Create a random client ID (may want to keep this static, actually. a good ID could be the ESP8266's MAC address)
#ifdef DEBUG
        Serial.print("Attempting MQTT connection...");
#endif
        char clientId[] = "ESP8266Client-1";
        //int rand_id = random(0xffff), HEX); // replace with MAC address?

        // Attempt to connect
        if (MQTTclient.connect(clientId))
        {
#ifdef DEBUG
            Serial.println("connected");
#endif
            digitalWrite(ERR_LED, LOW);
        }
        else
        {
#ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(MQTTclient.state());
            Serial.println(" try again in 5 seconds");
#endif
            error_blink_leds(400); // blink LEDS 4 times with 400ms as the delay
        }
    }
}

int get_topic_and_topic_msg(char *ser_data)
{

    TOPIC = strtok(ser_data, " "); /* data comes in from arduino as "/DHN/<network #>/<node id>/<topic name> <data>"; delim is space */
    if (TOPIC == NULL)
        return 0;

    TOPIC_DATA = strtok(NULL, " ");
    if (TOPIC_DATA == NULL)
    {
#ifdef DEBUG
        Serial.print("Error: missing data point for topic ");
        Serial.print(TOPIC);
        Serial.println("\nPublishing terminated.");
#endif
        return 0;
    }
    else
    {
        if (strcmp(TOPIC, "error_messages") == 0){
            sprintf(TOPIC_MSG, "%s %s %s", ser_data, TIME, DATE);
        } else {
            sprintf(TOPIC_MSG, "%s %s %s", TOPIC_DATA, TIME, DATE); /* put a timestamp on data */
        }
        return 1;
    }
}

void set_timestamp()
{
    sprintf(TIME, "%s", timeClient.getFormattedTime().c_str());
    sprintf(DATE, "%s", timeClient.getFormattedDate().c_str());
}

void print_timestamp()
{
   print_date();
   print_time(); 
}

void print_date(){
    Serial.print(DATE);
    Serial.print('\n');
    Serial.flush();
}

void print_time(){
    Serial.print(TIME);
    Serial.print('\n');
    Serial.flush();
}

void error_blink_leds(int ms)
{
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
    delay(ms);
    digitalWrite(ERR_LED, LOW);
    delay(ms);
    digitalWrite(ERR_LED, HIGH);
}