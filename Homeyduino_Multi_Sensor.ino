/*
 * version 1.1
 * 
 * Homeyduino Multi Sensor reporting temperature, humidity and luminance to Homey  
 * Issues and (especially) pull requests are welcome: https://github.com/MagnusPer/Homeyduino_MultiSensor
 * 
 * Athom Homey: https://www.athom.com/en/
 * 
 * Arduino library for communicating with Homey: https://github.com/athombv/homey-arduino-library
 * Arduino library for Adafruit TSL2561 Luminance sensor: https://github.com/adafruit/Adafruit_TSL2561
 * Arduino library for Adafruit Unified Sensor: https://github.com/adafruit/Adafruit_Sensor
 * Arduino library for DHT22 sensor for esp8266: https://github.com/beegee-tokyo/DHTesp
 * 
 */

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <DHTesp.h>
#include <Homey.h>

DHTesp dht;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

const char* wifiSSID = "";           /* your Wifi network SSID */
const char* wifiPassword = "";       /* your WiFi network password */      

int currentLuminance;
int previousLuminance = 0;
float currentHumidity;
float previousHumidity = 0;
float currentTemperature;
float previousTemperature = 0;
unsigned long previousMillis = 0;      
const unsigned long reportInterval = 600000;        /* Time in milliseconds between reports to Homey (600 000ms = 10min or 60 000ms = 1min) */
int thresholdLuminance = 100;                        /* Treshold level in luminance (Lux) to send report to Homey (current-previous > treshold) */
float thresholdHumidity = 1;                        /* Treshold level in humidity (H) to send report to Homey */
float thresholdTemperature = 0.5;                   /* Treshold level in celsius (*C) to send report to Homey */

/**************************************************************************/
/* Configures the gain and integration time for the TSL2561 Light Sensor  */
/**************************************************************************/

void configureLuminanceSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);         /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);        /* 16x gain ... use in low light to boost sensitivity */
     tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);     /* medium resolution and speed   */
     tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);     /* 16-bit data but slowest conversions */
}

/**************************************************************************/
/* Get a new temperature sensor event                                     */
/**************************************************************************/

void Temperature() {  /* Get a new temperature sensor event */
  currentTemperature = dht.getTemperature();
  //Serial.print(currentTemperature); Serial.print(" *C, ");
  } 

/**************************************************************************/
/* Get a new humidity sensor event                                        */
/**************************************************************************/

void Humidity(){   /* Get a new humidity sensor event */
  currentHumidity = dht.getHumidity();
  //Serial.print(currentHumidity); Serial.print(" H, ");
  }

/**************************************************************************/
/* Get a new luminance sensor event                                           */
/**************************************************************************/

void Luminance(){   
  sensors_event_t event;
  tsl.getEvent(&event);
  currentLuminance = event.light;
  //Serial.print(currentLuminance); Serial.println(" lux");
  }

/**************************************************************************/
/*  Arduino setup function (automatically called at startup)              */
/**************************************************************************/

void setup(void) 
{
 Serial.begin(115200);
  Serial.println();
  Serial.println();
 
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);
  IPAddress ip(192,168,1,206);                  /* Configure static IP address */
  IPAddress gateway(192,168,1,1);               /* Configure static IP address */
  IPAddress subnet(255,255,255,0);              /* Configure static IP address */
  WiFi.config(ip, gateway, subnet);             /* Set static IP address */
  
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.localIP());

  /* Initiate and starts the Homey interface  */
  Homey.begin("MultiSensor");
  Homey.setClass("sensor");
  Homey.addCapability("measure_temperature");
  Homey.addCapability("measure_humidity");
  Homey.addCapability("measure_luminance");
  
  /* Initialise the Light Sensor */
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  /* Setup the light sensor gain and integration time */
  configureLuminanceSensor();

  /* Setup the DHT22 sensor to pin D5 */
  dht.setup(D5, DHTesp::DHT22); 
  
}


void loop(void) 
{  
  Homey.loop();
 
  /* Get fresh sensor values */
  Temperature();
  Humidity();
  Luminance();
  
  /* Report to Homey every reportInterval */
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > reportInterval) {
    previousMillis = currentMillis;
    
    /*Serial.print(currentTemperature); Serial.print(" *C, ");
    Serial.print(currentHumidity); Serial.print(" H, ");
    Serial.print(currentLuminance); Serial.println(" lux");*/
    
    Homey.setCapabilityValue("measure_temperature", (float) currentTemperature);
    Homey.setCapabilityValue("measure_humidity", (float) currentHumidity);
    Homey.setCapabilityValue("measure_luminance", (int) currentLuminance);
    }
  
  /* Report to Homey when threshold level is met */
    if ((currentTemperature - previousTemperature >= thresholdTemperature) || (previousTemperature - currentTemperature >= thresholdTemperature)) {
      previousTemperature = currentTemperature;
      Homey.setCapabilityValue("measure_temperature", (float) currentTemperature);
      }

  if ((currentHumidity - previousHumidity >= thresholdHumidity) || (previousHumidity - currentHumidity >= thresholdHumidity)) {
      previousHumidity = currentHumidity;
      Homey.setCapabilityValue("measure_humidity", (float) currentHumidity);
    }
  
  if ((currentLuminance - previousLuminance >= thresholdLuminance) || (previousLuminance - currentLuminance >= thresholdLuminance)) {
      previousLuminance = currentLuminance;
      Homey.setCapabilityValue("measure_luminance", (int) currentLuminance);
    }

  delay(500);     /* To set a minimum reportInterval if the sensordata change fast*/
}
