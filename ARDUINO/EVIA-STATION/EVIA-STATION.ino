// EPHL - EVIA WEATHER STATION - TeZ 2021
/////////////////////////////////////////
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <ACROBOTIC_SSD1306.h>
#include "NETWORK.h"
#include "NTP.h"
#include <WiFiMulti.h>
#include <HTTPClient.h>

#define DHTPIN 25    
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS = 4000;

WiFiMulti wifiMulti;
HTTPClient myhttp;
String dweeter = "http://dweet.io/dweet/for/ephl?temper=";

float temperature = 0.23;
float humidity = 0.23;

unsigned long previous_time = 0;
int delcheck = 30000;

////////////////////////////////////////
void initLCD(){
  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(1,2);              // Set cursor position, start of line 0
  oled.putString("HELLO");
  oled.setTextXY(2,2);              // Set cursor position, start of line 1
  oled.putString("HUMAN!");
  oled.setTextXY(4,2);              // Set cursor position, start of line 2
  oled.putString("====> ");
//  oled.setTextXY(7,9);             // Set cursor position, line 2 10th character
  oled.putString(":)");
  
}

//////////////////////////////////////////////
void DHTread() {

  oled.clearDisplay();     // Clear screen

  
 // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    oled.setTextXY(1,2);              // Set cursor position, start of line 0
    oled.putString("Temp: ");
    oled.putString(String(event.temperature));

  }

  temperature = event.temperature;
  
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));

    oled.setTextXY(4,2);              // Set cursor position, start of line 1
    oled.putString("Hum: ");
    oled.putString(String(event.relative_humidity));
  }

  humidity = event.relative_humidity;

}



/////////////////////////
void timePump(){

  oled.clearDisplay();     // Clear screen

  timeClient.update();

  String myDayString = daysOfTheWeek[timeClient.getDay()];
//  myDayString = myDayString.substring(0, 3);
  String myCurrentDay = myDayString;
  
  String myTimeString = timeClient.getFormattedTime();
  String formattedDate = timeClient.getFormattedDate();

  int splitT = formattedDate.indexOf("T");
  String dayStamp = formattedDate.substring(0, splitT);
  int splitX = myTimeString.indexOf(":");
  String timeStamp = myTimeString.substring(0, splitX);
 


  Serial.println("=+=+=+=+=+=+=+=+=+=");
  Serial.println(myCurrentDay);
  Serial.println("=+=+=+=+=+=+=+=+=+=");

  oled.setTextXY(1,2);              
  oled.putString("it's ");
  oled.putString(String(myDayString));
  oled.setTextXY(3,2);
  oled.putString(String(dayStamp));
  oled.setTextXY(5,2); 
  oled.putString(String(myTimeString));
  
}


///////////////////////////////////////////////////
void GetDweet(String mysensor){

    //   posthttp.begin("http://dweet.io/get/latest/dweet/for/tez-sensor-02"); // here connects to GET DWEET data

          String sensorUrl = "http://dweet.io/get/latest/dweet/for/";
          sensorUrl.concat(mysensor);
       
          myhttp.begin(sensorUrl); // here connects to GET DWEET data
 
        // start connection and send HTTP header
        int httpCode = myhttp.GET();
 
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
           // Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = myhttp.getString();
               // Serial.println(payload);
            }
        } else {
           // Serial.printf("[HTTP] GET... failed, error: %s\n", myhttp.errorToString(httpCode).c_str());
        }

       myhttp.end();

}



///////////////////////////////////////////////////
  void PostDweet(String mysensor, float myvalue){

        dweeter = "http://dweet.io/dweet/for/ephl?";//
        dweeter.concat(mysensor); 
        dweeter.concat("="); 
        dweeter.concat(myvalue); // attach the variable (here you change the parameter with your variable)
        myhttp.begin(dweeter); // here connects and POSTs data to DWEET at the same time
        // Serial.print("POSTed >> "); 
        // USE_SERIAL.println(dweeter);


          // GET SESSION
        // start connection and send HTTP header
        int httpCode = myhttp.GET();
 
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
           // Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = myhttp.getString();
               // Serial.println(payload);
            }
        } else {
          //  Serial.printf("[HTTP] GET... failed, error: %s\n", myhttp.errorToString(httpCode).c_str());
        }

      // END GET SESSION

        //GetDweet(mysensor);
        
        myhttp.end();


/*
        dweeter = "http://dweet.io/dweet/for/heartbeat?myvalue="; // this is fixed, DO NOT CHANGE
        prog +=1; // this is the variable to be attached to the Dweet Post, can be your sensor value
        dweeter.concat(prog); // attach the variable (here you change the parameter with your variable)
        posthttp.begin(dweeter); // here connects and POSTs data to DWEET at the same time
        USE_SERIAL.print("POSTed >> "); 
        USE_SERIAL.println(dweeter);
  */

}

///////////////////////////////////////////////
void CheckWiFi(){
   unsigned long current_time = millis();
   unsigned long diff_time = (current_time - previous_time); 
  // checking for WIFI connection
  if ( (WiFi.status() != WL_CONNECTED) && (diff_time >= delcheck) ){ 
    Serial.print(millis());
    Serial.println("Reconnecting to WIFI network");
    WiFi.disconnect();
    WiFi.reconnect();
    previous_time = current_time;
  }
  
}

////////////////////////////////////////////
void setup() {

  delay(1000);
  WiFiConnect();
  delay(1000);

  Wire.begin();  
  initLCD();
  
  Serial.begin(115200);
  
  dht.begin(); // start temperature/humidity sensor

  timeClient.begin(); // start NTP client
  
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

  // Set delay between sensor readings based on sensor details.
  // delayMS =  2000; // sensor.min_delay / 1000;
 
   PostDweet("temper", 123); // first call and init to the http socket
 
  
}


//////////////////////////////////////////////
void loop() {

    CheckWiFi(); // check that WiFi connection is active
    
    // Delay between measurements.
    delay(delayMS);
    DHTread(); // read DHT sensor
    delay(delayMS);
    timePump(); // read NTP intenet time
    delay(delayMS);
    
    PostDweet("temperature", temperature); // first call and init to the http socket
    delay(1000);
    PostDweet("humidity", humidity); // first call and init to the http socket
    delay(1000);

    initLCD();
    delay(2000);
 
    
}
