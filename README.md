# Homeyduino_MultiSensor
An Homeyduino (esp8266) multi sensor reporting temperature, humidity and luminance to Homey.

## Features
To reduce network trafik and load for Homey the multi sensor only report status either after a predefined interval or when the configurable threshold is met.

| Parameter              | Default value  | Description                                                          |
| ---------------------- | -------------- | -------------------------------------------------------------------- |
| `reportInterval`       | 60 000 ms      | Time in milliseconds between reports to Homey (60 000ms = 1min)      |
| `thresholdTemperature` | 0.5            | Threshold level in celsius (*C) to send report to Homey              |
| `thresholdHumidity`    | 1              | Threshold level in humidity (H) to send report to Homey              |
| `thresholdLuminance`   | 100            | Threshold level in luminance (Lux) to send report to Homey           |

## Release Notes
version 1.0 - initial release

## Installation

## Wiring

![Homeyduino_MultiSensor](https://github.com/MagnusPer/Homeyduino_MultiSensor/blob/master/docs/Homeyduino_MultiSensor.jpg)

## References
Athom Homey: https://www.athom.com/en/

Arduino library for communicating with Homey: https://github.com/athombv/homey-arduino-library

Arduino library for Adafruit TSL2561 Luminance sensor: https://github.com/adafruit/Adafruit_TSL2561

Arduino library for Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor

Arduino library for DHT22 sensor for esp8266: https://github.com/beegee-tokyo/DHTesp
