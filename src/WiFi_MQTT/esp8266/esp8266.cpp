#include "esp8266.h"

// WiFi objects
IPAddress staticIP(192, 168, 1, 68);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
IPAddress MQTTbroker(192, 168, 1, 253); 
// For timestamp
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

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
    pinMode(LED_BUILTIN, OUTPUT); // configure LED on ESP8266 for debugging use
    Serial.begin(115200); // begin serial monitoring
    setup_Wifi(); // configure wifi settings
    timeClient.begin(); // begin the NTP Client 
    MQTTclient.setServer(MQTTbroker, 1883); // start the MQTT client
    //MQTTclient.setCallback(callback); // function to handle subscription updates
}

void loop()
{
    timeClient.update(); // updates the time based on an interval (see .h file)
    if (!MQTTclient.connected()) // connection broken?
        reconnect_to_broker();
    MQTTclient.loop();

    if (Serial.available() > 0) // is data is available from Arduino?
    {
        String ser_data = Serial.readStringUntil('\n'); // read arduino data from serial port
        if (get_topic_and_topic_msg(ser_data)) // topic name and data legible?
        {
            MQTTclient.publish(TOPIC, TOPIC_MSG);  // publish to topic
        }
    }
}

void setup_Wifi()
{
    delay(3000);
    digitalWrite(LED_BUILTIN, ESP8266_LED_OFF);
    WiFi.hostname(HOSTNAME);
    if (!WiFi.config(staticIP, gateway, subnet, primaryDNS, secondaryDNS))
    {
        //Serial.println("STA failed to configure");
        error_blink_leds(200);
    }

    //  Serial.print("Connecting...");
    WiFi.begin(SSID, SSID_PW);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        //    Serial.print(".");
    }
    digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
    //  Serial.print("Connected!\nIP Address: ");
    //  Serial.println(WiFi.localIP());
}

// function used to reconnect to MQTT broker in case of a lost cnxn
void reconnect_to_broker()
{
    // Loop until we're reconnected
    while (!MQTTclient.connected())
    {
        // Create a random client ID (may want to keep this static, actually. a good ID could be the ESP8266's MAC address)
        //Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX); // replace with MAC address?

        // Attempt to connect
        if (MQTTclient.connect(clientId.c_str()))
        {
            //Serial.println("connected");
            digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
        }
        else
        {
            //Serial.print("failed, rc=");
            //Serial.print(MQTTclient.state());
            //Serial.println(" try again in 5 seconds");
            error_blink_leds(400); // blink LEDS 4 times with 400ms as the delay
        }
    }
}

int get_topic_and_topic_msg(String ser_data)
{
    if (sizeof(ser_data) >= DATA_BUFFER_SIZE)
        return 0;

    char data_buf[DATA_BUFFER_SIZE];
    ser_data.toCharArray(data_buf, DATA_BUFFER_SIZE);
    TOPIC = strtok(data_buf, " "); // data comes in from arduino as "/DHN/<network #>/<node id>/<topic name> <data>"; delim is space
    if (TOPIC == NULL)
        return 0;

    TOPIC_DATA = strtok(NULL, " ");
    if (TOPIC_DATA == NULL)
    {
        //    Serial.print("Error: missing data point for topic ");
        //    Serial.print(topic);
        //    Serial.println("\nPublishing terminated.");
        return 0;
    }
    else
    {
        timestamp_data(); // put a timestamp on data
        return 1;
    }
}

void timestamp_data()
{
    String timestamp = String(daysOfTheWeek[timeClient.getDay()]) + String(' ') + String(timeClient.getHours()) + String(':') + String(timeClient.getMinutes()) + String(':') + String(timeClient.getSeconds());
    sprintf(TOPIC_MSG, "%s  %s", TOPIC_DATA, timestamp.c_str());
}

void error_blink_leds(int ms)
{
    digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_OFF);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_OFF);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_OFF);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_ON);
    delay(ms);
    digitalWrite(LED_BUILTIN, ESP8266_LED_OFF);
    delay(ms);
}