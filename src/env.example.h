#ifndef SETTINGS_H // include guard
#define SETTINGS_H

// Update these with values suitable for your network.
const char* node_name = "backyard";
const char* wlan_ssid = "A-B-C-D";
const char* wlan_pass = "........";
IPAddress mqtt_server(10, 0, 0, 100);
const int mqtt_port = 1883;
const char* mqtt_user = "guest";
const char* mqtt_pass = "guesswhat";
const char* mqtt_topic = "sensor/backyard";

// Pin for reading DHT22
#define SENSOR_GPIO 2

// debug flash abd print to indicate collect and sending data, undef to disable
#define LED 2
#define SERIAL_PRINT
//#undef LED
//#undef SERIAL_PRINT

// retry count for WiFi (dis)connect, wait for 500ms between retries
// connecting will reset the board if did not success at all
#define MAX_WIFI_RETRIES 60

// sampling period, sleep time, in milliseconds
unsigned long sampling_period = 600000;

#endif