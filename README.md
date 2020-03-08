# MultiSensor
An [NodeMCU](https://www.nodemcu.com/index_en.html) (esp8266) based multi sensor reporting temperature, humidity and luminance via MQTT topics. Perfect to be added to a HomeAutomation system 

## Features
To reduce network trafik and network load the multi sensor only report status after a predefined interval or when the configurable threshold is met.

| Parameter              | Default value  | Description                                                          |
| ---------------------- | -------------- | -------------------------------------------------------------------- |
| **reportInterval**       | 600 000 ms   | Time in milliseconds between reports (600 000ms = 10min)             |
| **thresholdTemperature** | 0.5          | Threshold level in celsius (*C) to send report to Homey              |
| **thresholdHumidity**    | 1            | Threshold level in humidity (H) to send report to Homey              |
| **thresholdLuminance**   | 100          | Threshold level in luminance (Lux) to send report to Homey           |

## Installation
Needed Arduino Libraries to be included in [IDE](https://www.arduino.cc/en/Main/Software). Install them either from GitHub repositories directly or within the IDE application itself **Sketch > Import Library** 

| Library                            | Link to GitHub                                      |
| ---------------------------------- | --------------------------------------------------- |
| library for MQTT                   |  https://github.com/knolleary/pubsubclient          |
| Adafruit TSL2561 Luminance sensor  |  https://github.com/adafruit/Adafruit_TSL2561       |
| Adafruit Unified Sensor            |  https://github.com/adafruit/Adafruit_Sensor        |
| DHT22 sensor for esp8266           |  https://github.com/beegee-tokyo/DHTesp             |


## MQTT Topics
MQTT Topics to be sent to the Broker 

| Topic                              | Description                                         |
| ---------------------------------- | --------------------------------------------------- |
| MultiSensor/Luminance              |  sensor value in lux                                |
| MultiSensor/Temperature            |  sensor value in celsius                            |
| MultiSensor/Humidity               |  sensor value in humidity                           |

## Wiring

![Homeyduino_MultiSensor](https://github.com/MagnusPer/Homeyduino_MultiSensor/blob/master/images/Homeyduino_MultiSensor.jpg)

## Release Notes
- version 1.0 - First release
- version 1.1 - Minor updates | increased reportInterval and cleaned up some code

## Future improvments
- Sensor calibration configuration parameters
- Minimum reportInterval configuration parameter to minimize reports when Luminance quickly change 

## References
- Adafruit TSL2561: https://www.adafruit.com/product/439
- Adafruit DHT22: https://www.adafruit.com/product/385 
- NodeMcu: http://nodemcu.com/index_en.html
