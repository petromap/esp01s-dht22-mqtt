#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <DHTesp.h>

#include "env.h"

WiFiClient net;
WiFiUDP netUDP;
NTPClient timeClient(netUDP, "europe.pool.ntp.org", 0, 300000);
PubSubClient client(net);
DHTesp dht;
unsigned long prev_report;

void WiFi_Init() {
  WiFi.forceSleepWake();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wlan_ssid, wlan_pass);
  #ifdef SERIAL_PRINT
    Serial.print(F("Connecting to "));
    Serial.println(wlan_ssid);
  #endif
  int conn_tries = 0;
  while ((WiFi.status() != WL_CONNECTED) && (conn_tries++ < MAX_WIFI_RETRIES)) {
    #ifdef SERIAL_PRINT
      Serial.print('.');
    #endif
    delay(500);
  }
  if (conn_tries >= MAX_WIFI_RETRIES) {
    #ifdef SERIAL_PRINT
      Serial.println(F(" !!! No WiFi connection ...restarting."));
    #endif
    ESP.reset();
    return;
  }
  #ifdef SERIAL_PRINT
    Serial.println(F(""));
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
  #endif
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

bool WiFi_Off() {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    yield();

    // Give it a couple of seconds before returning a fail.
    int conn_tries = 0;
    while ((WiFi.status() == WL_CONNECTED) && (conn_tries++ < MAX_WIFI_RETRIES)) {
        delay(500);
        #ifdef SERIAL_PRINT
          Serial.print(F("."));
        #endif
    }
    #ifdef SERIAL_PRINT
      Serial.println(F(""));
    #endif
    if (WiFi.status() != WL_CONNECTED) {
        return true;
    } else {
        return false;
    }
}

void initBroker() {
  client.setServer(mqtt_server, mqtt_port);
}

void setup() {
  // debug print
  #ifdef SERIAL_PRINT
  Serial.begin(115200);
  delay(1000);
  Serial.setDebugOutput(true);
  #endif

  // call for network and broker
  //WiFi_Init();
  initBroker();
  timeClient.begin();

  // setup sensor on GPIOx
  dht.setup(SENSOR_GPIO, DHTesp::DHT22);

  // debug flash
  #ifdef LED
  pinMode(LED, OUTPUT);
  // indicate we are going to alive
  digitalWrite(LED, LOW);
  delay(15000);
  digitalWrite(LED, HIGH);
  #endif

  prev_report = millis() - sampling_period;
}

void reconnectBroker() {
  int conn_tries = 0;
  while ((!client.connected()) && (conn_tries++ < 6)) {
    #ifdef SERIAL_PRINT
      Serial.print(F("Attempting MQTT connection..."));
    #endif
    if (client.connect(node_name, mqtt_user, mqtt_pass)) {
    #ifdef SERIAL_PRINT
      Serial.println(F(" connected."));
    #endif
    } else {
      #ifdef SERIAL_PRINT
        Serial.print(F(" failed, rc="));
        Serial.print(client.state());
        Serial.println(F(" let's try again in 5 seconds"));
      #endif
      delay(2000);
    }
  }
}

unsigned long previousWifiStatusMillis = 0;
void print_wifi_status() {
  #ifdef SERIAL_PRINT
  //print the Wi-Fi status max. every 30 seconds
  unsigned long currentMillis = millis();
  if (currentMillis - previousWifiStatusMillis >= 30000){
    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println(F("Configured SSID cannot be reached"));
        break;
      case WL_CONNECTED:
        Serial.println(F("Connection successfully established"));
        break;
      case WL_CONNECT_FAILED:
        Serial.println(F("Connection failed"));
        break;
      case WL_CONNECTION_LOST:
        Serial.println(F("Connection lost"));
        break;
      default:
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print(F("RSSI: "));
    Serial.println(WiFi.RSSI());
    previousWifiStatusMillis = currentMillis;
  }
  #endif
}

void loop() {
  //if ((millis() - prev_report) > sampling_period) {
    delay(1000);
  //  return;
  //}

  // Wake up modem as we've swithed it off, see end of the loop
  WiFi_Init();

  print_wifi_status();
  delay(100); // stabilize, comment out if not switching modem on/off

  if (!client.connected()) {
    reconnectBroker();
  }
  client.loop();
  timeClient.update();

  unsigned long epoch_time = timeClient.getEpochTime();
  if (!timeClient.isTimeSet()) {
    // this should be handled on receiver, send data to see node is alive
    epoch_time = 0;
  }

  // Collect values and write the doc
  DynamicJsonDocument doc(512);

  unsigned long read_start = millis();

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);
  float dewPoint = dht.computeDewPoint(temperature, humidity, false);

  doc[F("node")] = node_name;
  doc[F("time")] = epoch_time;
  JsonObject values = doc.createNestedObject(F("values"));
  values[F("RH")] = humidity;
  values[F("T")] = temperature;
  values[F("HI")] = heatIndex;
  values[F("TD")] = dewPoint;

  if (timeClient.isTimeSet()) {
    doc[F("read_time")] = millis() - read_start;
  }
  doc[F("read_state")] = dht.getStatus();
  doc[F("rssi")] = WiFi.RSSI();
  doc[F("free_heap")] = ESP.getFreeHeap();
  doc[F("heap_frag")] = ESP.getHeapFragmentation();

  #ifdef SERIAL_PRINT
  serializeJson(doc, Serial);
  #endif

  // publish the doc
  String payload;
  serializeJson(doc, payload);
  if (client.publish(mqtt_topic, payload.c_str())) {
    #ifdef SERIAL_PRINT
      Serial.println(F(" - published."));
    #endif
  } else {
    #ifdef SERIAL_PRINT
      Serial.println(F(" - publish failed!"));
    #endif
  }
  client.loop();

  #ifdef LED
  digitalWrite(LED, LOW);
  delay(500);
  digitalWrite(LED, HIGH);
  #endif

  // Switch modem off, for a long long wait
  WiFi_Off();
/*
  if (sampling_period < abs(dht.getMinimumSamplingPeriod())) {
    delay(dht.getMinimumSamplingPeriod());
  } else {
    delay(sampling_period);
  }*/
}