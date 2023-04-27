# esp01s-dht22-mqtt
Simple ESP-01s + DHT22 sensor, communicating over MQTT to data collecting service.

## Configure project and build

Copy `src/env.example.h` to `src/env.h`. Edit settings in it, especially the ones for WLAN and MQTT to match your environment. 

This is a PlatformIO project and easiest way to build and upload is using commands provided by this platform.

TODO: explain what and where to sending data

TODO: explain the format of message

TODO: explain libraries


TODO: these are from DHT library:

http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml

https://wahiduddin.net/calc/density_algorithms.htm

**Note:** In this implementation WiFi is switched to on and off in the ```loop()``` as it is going to wait for a long time before next sampling. 
For 'constant' measuring comment out ```Wifi_Init()``` and ```Wifi_Off()``` from ```loop()``` and mind to change the ```sampling_period```.

