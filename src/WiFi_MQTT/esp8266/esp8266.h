#define UTC_OFFSET_SECONDS -18000 /* offset for Atlantic Time (daylight savings) */
#define UPDATE_INTERVAL 360000 /* ms, updates every minute */
#define MSG_BUFFER_SIZE 100 /* bytes, for PubSubClient */
#define DATA_BUFFER_SIZE 200
#define ERR_LED 12
#define ARDUINO_RX 2
#define ARDUINO_TX 4
#define TIMESTAMP_REQUEST "t"
//#define DEBUG /* uncomment for debug prints */

/* WiFi settings */
#define SSID "Inty-plus"
#define SSID_PW "U5t\\<a%>q=ikhwLY"
#define HOSTNAME "ESP8266-1"

/* prototypes */
void error_blink_leds(int ms);
void setup_Wifi();
void reconnect_to_broker();
int get_topic_and_topic_msg(char *ser_data);
void set_timestamp();
void print_timestamp();
void print_date();
void print_time();