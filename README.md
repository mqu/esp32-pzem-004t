Energy monitoring system based on [ESP32](https://esp32.info/), PZEM-004, Influxdb, [Chronograf](https://www.influxdata.com/time-series-platform/chronograf/). This hardware solution is cheap and a good alternative to [Linky](https://fr.wikipedia.org/wiki/Linky) (French device from [ENEDIS](https://fr.wikipedia.org/wiki/Enedis), an Electricity Distribution Company). It gives you the ability to own your private data and allow advanced usage, including monitoring energy consomption in real time.

* ESP32 is a very capacitive micro-controler with integrated Wifi. It is successor of ESP8266, sharing same libraries. It is now [integrated](https://github.com/espressif/arduino-esp32) with Arduino IDE, giving simplicity of use and large ecosystem with many libraries.
* [PZEM-004-T](https://innovatorsguru.com/ac-digital-multifunction-meter-using-pzem-004t/) is a small module with a serial port, connecting current transformer coil to a digital meter.

Please look at Wiki. Source code comming soon.

Links :
* https://github.com/thorsten-l/ESP8266-PZEM004T : very complete firmware with REST API, OTA, OpenHAB, Web Interface.
