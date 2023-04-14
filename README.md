# esp01s-dht22-mqtt
Simple ESP-01s + DHT22 sensor, communicating over MQTT to data collecting service.

TODO: explain how to configure, the env.h

TODO: explain what and where to sending data

TODO: explain the format of message

TODO: explain libraries


TODO: these are from DHT library:

http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml

https://wahiduddin.net/calc/density_algorithms.htm

**Note:** In this implementation WiFi is switched to on and off in the ```loop()``` as it is going to wait for a long time before next sampling. 
For 'constant' measuring comment out ```Wifi_Init()``` and ```Wifi_Off()``` from ```loop()``` and mind to change the ```sampling_period```.

