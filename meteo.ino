#include <WiFi.h>
#include "ThingSpeak.h"
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

#define SEALEVELPRESSURE_HPA (1013.25)
 
const char* ssid = "xxxxxxxxx";   // Your WIFI network SSID (name)
const char* password = "xxxxxxxxx";   // Your WIFI network password
 
WiFiClient  client;
 
unsigned long myChannelNumber = xxxxxxxxxxx; // SECRET_CH_ID
const char * myWriteAPIKey = "xxxxxxxxxxxxxxxxxx"; SECRET_WRITE_APIKEY
 
// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;
 
// Variables readings
float temperatureC;
float humidity;
float pressure;
float altitude;
 
// Create a sensor object
Adafruit_BME280 bme; // BME280 connect to ESP32 I2C (GPIO 21 = SDA, GPIO 22 = SCL)
 
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}
 
void setup() {
  Serial.begin(115200);  //Initialize serial
  initBME();
   
  WiFi.mode(WIFI_STA);   
   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
     
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
 
    // Get a new temperature reading
    temperatureC = bme.readTemperature();
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureC);
    humidity = bme.readHumidity();
    Serial.print("Humidity (%): ");
    Serial.println(humidity);
    pressure = bme.readPressure() / 100.0F;
    Serial.print("Pressure (hPa): ");
    Serial.println(pressure);
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print("Altitude (m): ");
    Serial.println(altitude);    
   
 
    // set the fields with the values
    ThingSpeak.setField(1, temperatureC);
    //ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, pressure);
    ThingSpeak.setField(4, altitude);
     
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
 
    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}
