#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "realme 3i";
const char* password = "thepunisher";
const char* thingSpeakApiKey = "HOIGPGYQPL1PJY3J";
const long channelID = 2318108;

const int pirPin = 4;  // PIR sensor input pin

bool motionDetected = false;

WiFiClient client;

void setup() {
  pinMode(pirPin, INPUT);
  
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  ThingSpeak.begin(client);
}

void loop() {
  int pirValue = digitalRead(pirPin);
  
  if (pirValue == HIGH) {
    if (!motionDetected) {
      Serial.println("Motion Detected!");
      motionDetected = true;
      // Send data to ThingSpeak
      sendToThingSpeak(1);
    }
  } else {
    if (motionDetected) {
      Serial.println("Motion Stopped");
      motionDetected = false;
      // Send data to ThingSpeak
      sendToThingSpeak(0);
    }
  }

  delay(1000);  // Adjust the delay as needed
}

void sendToThingSpeak(int motion) {
  ThingSpeak.setField(1, motion);
  int status = ThingSpeak.writeFields(channelID, thingSpeakApiKey);

  if (status == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.println("Failed to send data to ThingSpeak. Status code: " + String(status));
  }
}
