/*
 * version 1.1
 * 
 * Multi Sensor reporting temperature, humidity and luminance via MQTT 
 *
 * Arduino library for Adafruit TSL2561 Luminance sensor:   v1.0.3  https://github.com/adafruit/Adafruit_TSL2561
 * Arduino library for Adafruit Unified Sensor:             v1.0.3  https://github.com/adafruit/Adafruit_Sensor
 * Arduino library for DHT22 sensor for esp8266:            v1.0.3  https://github.com/beegee-tokyo/DHTesp
 * Arduino library for MQTT:                                v2.3.0  https://github.com/knolleary/pubsubclient
 * 
 * 
 *  To increase the report interval when the light is low (...when the automation is used)
 *  TODO 1: Lux > 200 - Read every minute store 10 values remove largest and smallest and calculate the average
 *  TODO 2: Lux < 200 - Read every minute store 5 values remove largest and smallest and calculate the average
 *  HINT: Set a boolean in the read luminance if the lux is < 200 lux 
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <DHTesp.h>


// Constants
const char *wifi_ssid      = "SSID";
const char *wifi_pwd       = "PWD";
const char *wifi_hostname  = "MultiSensor";
const char* mqtt_server    = "IP";              // MQTT Server IP, your home MQTT server eg Mosquitto on RPi, or some public MQTT
const int mqtt_port        = 1883;              // MQTT Server PORT, default is 1883 but can be anything.
const char *mqtt_user      = "USER";            // User name in the MQTT Broker
const char *mqtt_pwd       = "PWD";             // Password in the MQTT Broker
String clientId            = "MultiSensor : " + String(ESP.getChipId(), HEX);
const char *ota_hostname   = "MultiSensor";

// MQTT Constants
const char* mqtt_LuminanceStatus_set_topic    = "MultiSensor/Luminance";
const char* mqtt_TemperatureStatus_set_topic  = "MultiSensor/Temperature";
const char* mqtt_HumidityStatus_set_topic     = "MultiSensor/Humidity";

// Global
int currentLuminance;
int previousLuminance               = 0;
float currentHumidity;
float previousHumidity              = 0;
float currentTemperature;
float previousTemperature           = 0;
unsigned long previousMillis        = 0;      
const unsigned long reportInterval  = 600000;   // Time in milliseconds between reports to MQTT broker (600000ms = 10min)
int thresholdLuminance              = 100;      // Treshold level in luminance (Lux) to send report to MQTT broker (current-previous > treshold)
float thresholdHumidity             = 1;        // Treshold level in humidity (H) to send report to MQTT broker
float thresholdTemperature          = 0.5;      // Treshold level in celsius (*C) to send report to MQTT broker


WiFiClient espClient;
PubSubClient client(espClient);

DHTesp dht;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**************************************************************************/
/* Setup WiFi connection                                                  */
/**************************************************************************/

void setup_wifi() {
  
  WiFi.hostname(wifi_hostname);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pwd);

  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.localIP());
}

/**************************************************************************/
/* Setup OTA connection                                                   */
/**************************************************************************/

void setup_ota() {

  // Set OTA Password, and change it in platformio.ini
  ArduinoOTA.setHostname(ota_hostname); 
  //ArduinoOTA.setPassword("ESP8266_PASSWORD");
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {
    if (error == OTA_AUTH_ERROR);          // Auth failed
    else if (error == OTA_BEGIN_ERROR);    // Begin failed
    else if (error == OTA_CONNECT_ERROR);  // Connect failed
    else if (error == OTA_RECEIVE_ERROR);  // Receive failed
    else if (error == OTA_END_ERROR);      // End failed
  });
  ArduinoOTA.begin();

}

/**************************************************************************/
/* Setup MQTT connection                                                   */
/**************************************************************************/

void reconnect() {

  // Loop until we're reconnected
  // Attempt to connect

    if (!client.connected()) {
      client.setServer(mqtt_server, mqtt_port);
      Serial.print("Connecting to MQTT server "); Serial.print(mqtt_server);

      while (!client.connected()) {
        if (client.connect(clientId.c_str(), mqtt_user, mqtt_pwd ))
        {
          Serial.println("\tconnected");
        } else {
          delay(500); 
          Serial.print(".");
        }
      } 
   }
}

/**************************************************************************/
/* Publish Luminance to MQTT broker                                       */
/**************************************************************************/

void publishLuminance(){

    client.publish(mqtt_LuminanceStatus_set_topic, String(currentLuminance).c_str(), true);
    
    /*char LuminanceChar[5];  
    String LuminanceString = String(currentLuminance);
    LuminanceString.toCharArray(LuminanceChar, LuminanceString.length()+1);
    client.publish(mqtt_LuminanceStatus_set_topic, LuminanceChar, true);*/
}

/**************************************************************************/
/* Publish Temperature to MQTT broker                                     */
/**************************************************************************/

void publishTemperature(){

    client.publish(mqtt_TemperatureStatus_set_topic, String(currentTemperature).c_str(), true);
    
   /* char TemperatureChar[5];  
    String TemperatureString = String(currentTemperature);
    TemperatureString.toCharArray(TemperatureChar, TemperatureString.length()+1);
    client.publish(mqtt_TemperatureStatus_set_topic, TemperatureChar, true);*/
}

/**************************************************************************/
/* Publish Humidity staus to MQTT broker                                  */
/**************************************************************************/

void publishHumidity(){

    client.publish(mqtt_HumidityStatus_set_topic, String(currentHumidity).c_str(), true);
    
    /*char HumidityChar[5];  
    String HumidityString = String(currentHumidity);
    HumidityString.toCharArray(HumidityChar, HumidityString.length()+1);
    client.publish(mqtt_HumidityStatus_set_topic, HumidityChar, true);  */
}

/**************************************************************************/
/* Initialise the Light Sensor TSL2561 Light Sensor                       */
/**************************************************************************/

void initiateLightSensor(){
  if(!tsl.begin())
  {
    /* There was a problem detecting the TSL2561 ... check your connections */
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}

/**************************************************************************/
/* Configures the gain and integration time for the TSL2561 Light Sensor  */
/**************************************************************************/

void configureLuminanceSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
     tsl.setGain(TSL2561_GAIN_1X);         // No gain ... use in bright light to avoid sensor saturation 
  // tsl.setGain(TSL2561_GAIN_16X);        // 16x gain ... use in low light to boost sensitivity 
  // tsl.enableAutoRange(true);            // Auto-gain ... switches automatically between 1x and 16x 
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      // fast but low resolution 
     tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);     // medium resolution and speed   
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);     // 16-bit data but slowest conversions 
}

/**************************************************************************/
/* Get a new temperature sensor event                                     */
/**************************************************************************/

void Temperature() {  
  currentTemperature = dht.getTemperature();
  //Serial.print(currentTemperature); Serial.print(" *C, ");
  } 

/**************************************************************************/
/* Get a new humidity sensor event                                        */
/**************************************************************************/

void Humidity(){   
  currentHumidity = dht.getHumidity();
  //Serial.print(currentHumidity); Serial.print(" H, ");
  }

/**************************************************************************/
/* Get a new luminance sensor event                                           */
/**************************************************************************/

void Luminance(){   
  sensors_event_t event;
  tsl.getEvent(&event);
  
  // Filter out values larger than 65000, because 65353 value means saturned sensor, bad measurement...
  // this may happen in direct sunlight to the sensor 
  
  if (event.light < 65000) { currentLuminance = event.light; }   
  //Serial.print(currentLuminance); Serial.println(" lux");
  }

/**************************************************************************/
/*  Arduino setup function (automatically called at startup)              */
/**************************************************************************/

void setup(void) 
{
 Serial.begin(115200);
 setup_wifi();
 setup_ota();
 initiateLightSensor();         // initiate the light sensor 
 configureLuminanceSensor();    // Setup the light sensor gain and integration time 
 dht.setup(D5, DHTesp::DHT22);  // Setup the DHT22 sensor to pin D5 
}


void loop(void) 
{  
  if (!client.connected()) { reconnect(); }
  client.loop();
  ArduinoOTA.handle();
 
  // Get fresh sensor values
  Temperature();
  Humidity();
  Luminance();
  
  // Report status to MQTT broker every reportInterval 
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > reportInterval) {
    previousMillis = currentMillis;
    
     publishLuminance();
     publishTemperature();
     publishHumidity();
    /*Serial.print(currentTemperature); Serial.print(" *C, ");
    Serial.print(currentHumidity); Serial.print(" H, ");
    Serial.print(currentLuminance); Serial.println(" lux");*/

    }
  
 
  // Report to MQTT broker when threshold level is met 
/*    
   if ((currentTemperature - previousTemperature >= thresholdTemperature) || (previousTemperature - currentTemperature >= thresholdTemperature)) {
      previousTemperature = currentTemperature;
      publishTemperature();
      }

   if ((currentHumidity - previousHumidity >= thresholdHumidity) || (previousHumidity - currentHumidity >= thresholdHumidity)) {
      previousHumidity = currentHumidity;
      publishHumidity();
    }
  
   if ((currentLuminance - previousLuminance >= thresholdLuminance) || (previousLuminance - currentLuminance >= thresholdLuminance)) {
      previousLuminance = currentLuminance;
      publishLuminance();
    }*/

  delay(500);     // To set a minimum reportInterval if the sensordata change fast
}
