#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "ThingSpeak.h"
WiFiClient  client;

unsigned long myChannelNumber = 1568660;
const char * myWriteAPIKey = "1HY5D9207RN5SMPD";

const char* ssid = "ZTE-adde77";   // your network SSID (name) 
const char* password = "885dfbad";   // your network password

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "479760abe644fb9f8f7bd341d9981199";
String city = "Chennai";
String countryCode = "IN";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
       Serial.println(serverPath);
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
         
   JSONVar temperature=myObject["main"]["temp"];
   JSONVar pressure=myObject["main"]["pressure"];
   JSONVar humidity=myObject["main"]["humidity"];
   JSONVar windspeed=myObject["wind"]["speed"];

   int tempThings=temperature;
   tempThings=tempThings-273;
   int pressureThings=pressure;
   int humidityThings=humidity;
   int windspeedThings=windspeed;

           ThingSpeak.setField(1, tempThings);
           ThingSpeak.setField(2, pressureThings);
           ThingSpeak.setField(3, humidityThings);
           ThingSpeak.setField(4, windspeedThings);

        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if(x == 200){
          Serial.println("Channel update successful.");
        }
        else{
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
     // Serial.println(myObject["main"]["temp"]);
      Serial.println(temperature);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(serverName);
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  return payload;
}
