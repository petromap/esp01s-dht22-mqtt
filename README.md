# esp01s-dht22-mqtt
This is ESP-01s + DHT22 sensor node, communicating over MQTT to data collecting service,
with NTP client, reconnecting and turning WIFI off and on.

## Configure project and build

Copy `src/env.example.h` to `src/env.h`. Edit settings in it, especially the ones for WLAN and MQTT to match your environment. 

This is a PlatformIO project and easiest way to build and upload is using commands provided by this platform.

## Payload

On each sampling time this node will read temperature and humidity from DHT22, calculate heat index and dew point and send these values with some additional status information as a JSON formatted message to MQTT broker.

Message format is adapted to [home-energy-monitoring](https://github.com/petromap/home-energy-monitoring-dev) and complete example is:
```JSON
{
"node": "node name",
"time": 1682963495,
"values":
{
  "RH": 75.5,
  "T": 27.2,
  "HI": 29,
  "TD": 22.5
},
"read_time": 5,
"read_state": 0,
"rssi": -82,
"free_heap": 0,
"heap_frag": 0
}
```
Sampling period (or frequency) can be configured by `sampling_period` in `env.h`.

**Note:** In this implementation WiFi is switched to on and off in the ```loop()``` as it is going to wait for a long time before next sampling. 
For 'constant' measuring comment out ```Wifi_Init()``` and ```Wifi_Off()``` from ```loop()``` and mind to change the ```sampling_period```.

