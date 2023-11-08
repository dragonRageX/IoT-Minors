#include <DHT.h>
#include <Arduino.h>
#include <HTTPClient.h> 
#include <Arduino_JSON.h> 
#include "ThingSpeak.h" //thingspeak

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// Insert your network credentials
#define WIFI_SSID "Note7pro"
#define WIFI_PASSWORD "kidi1086"
// Insert Firebase project API Key
#define API_KEY "AIzaSyCX9HXZwE8U1BUtM0ffuqXo6HSc954ASNA" //firebase
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://learning-a8f52-default-rtdb.asia-southeast1.firebasedatabase.app/" //firebase
#define DHTPIN 5     // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11   // Type of DHT sensor, can be DHT11, DHT22, or DHT21

/*********************thingspeakdetails********************************/
unsigned long myChannelNumber = 2299405; 
const char * myWriteAPIKey = "IZNZB03QP6BBEMZN";

//Define Firebase Data object
DHT dht(DHTPIN, DHTTYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
WiFiClient  client;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void setup(){
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop(){
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

/**********************************Thingspeak********************************/
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }


      delay(2000);
    
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
    
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) 
      {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      else
      {

        ThingSpeak.setField(3, h);
        ThingSpeak.setField(1, t);
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if(x == 200){
          Serial.println("Channel update successful.");
        }
        else{
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
        lastTime = millis();
      }
    

    
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print(f);
          Serial.println(F("°F"));
}


/************************************Firebase*****************************************************/
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/Temperature", temperature)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/Humidity", humidity)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

/**************************************StringthroughThingspeak**************************************************/

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


