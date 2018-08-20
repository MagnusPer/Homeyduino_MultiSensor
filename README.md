# Homeyduino_MultiSensor
An [NodeMcu](http://nodemcu.com/index_en.html) (esp8266) based multi sensor reporting temperature, humidity and luminance to [Homey](https://www.athom.com/en/).

## Features
To reduce network trafik and load for Homey the multi sensor only report status either after a predefined interval or when the configurable threshold is met.

| Parameter              | Default value  | Description                                                          |
| ---------------------- | -------------- | -------------------------------------------------------------------- |
| **reportInterval**       | 600 000 ms   | Time in milliseconds between reports to Homey (600 000ms = 10min)    |
| **thresholdTemperature** | 0.5          | Threshold level in celsius (*C) to send report to Homey              |
| **thresholdHumidity**    | 1            | Threshold level in humidity (H) to send report to Homey              |
| **thresholdLuminance**   | 50           | Threshold level in luminance (Lux) to send report to Homey           |

## Release Notes
version 1.0 - initial release

version 1.1 - minor updates - increased reportInterval and decreased threshold for luminance

## Installation
In the Homey install the [Homeyduino](https://apps.athom.com/app/com.athom.homeyduino) app 

Needed Arduino Libraries to be included in [IDE](https://www.arduino.cc/en/Main/Software). Install them either from GitHub repositories directly or within the IDE application itself **Sketch > Import Library** 

| Library                            | Link to GitHub                                      |
| ---------------------------------- | --------------------------------------------------- |
| Homey-arduino                      |  https://github.com/athombv/homey-arduino-library   |      
| Adafruit TSL2561 Luminance sensor  |  https://github.com/adafruit/Adafruit_TSL2561       |
| Adafruit Unified Sensor            |  https://github.com/adafruit/Adafruit_Sensor        |
| DHT22 sensor for esp8266           |  https://github.com/beegee-tokyo/DHTesp             |

## Wiring

![Homeyduino_MultiSensor](https://github.com/MagnusPer/Homeyduino_MultiSensor/blob/master/docs/Homeyduino_MultiSensor.jpg)

## References
Athom Homey: https://www.athom.com/en/

Adafruit TSL2561: https://www.adafruit.com/product/439

Adafruit DHT22: https://www.adafruit.com/product/385 

NodeMcu: http://nodemcu.com/index_en.html
