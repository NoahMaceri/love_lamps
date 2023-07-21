/*
 * Love Lamps Version 2
 * NMACERI
 */

// EXTERNAL LIBRARIES
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// INTERNAL LIBRARIES
#include "private.hpp"  // Passwords, keys, server info, etc.
#include "neopix.hpp"

// BOARD SPECIFIC
#define BOARD_ID "testb1"
#define BOARD_TARGET "testb2"
void process_message(unsigned short message_idx);

// MQTT

std::string mqtt_base_topic = "love_lamps";
std::string mqtt_topic_target = mqtt_base_topic + "/" + BOARD_TARGET;
std::string mqtt_topic_self = mqtt_base_topic + "/" + BOARD_ID;
WiFiClient mqtt_wifi_driver;
PubSubClient mqtt_client(mqtt_wifi_driver);
void mqtt_callback(char* topic, byte* payload, unsigned int length);

// UNIX EPOCH TIME
const char* ntpServer = "pool.ntp.org";
unsigned long epoch_get_time();

// NEOPIXEL
NeoPix* neopix;

void setup() {
  // SERIAL 
  Serial.begin(115200);
  
  // NEOPXIEL
  Serial.println("-- Strapping NeoPixel --");
  neopix = new NeoPix();
  if(!neopix->strapped) {
    Serial.println("Failed to strap NeoPixel");
  }
  Serial.println("-- NeoPixel Strapped --");

  // Wifi
  Serial.println("-- Strapping Wifi-- ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to the WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-- Wifi Strapped --");

  // MQTT
  Serial.println("-- Strapping MQTT --");
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setCallback(mqtt_callback);
  while (!mqtt_client.connected()) {
      String client_id = "cs-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
      if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Conneceted to MQTT broker");
      } else {
          Serial.print("Failed to connect, rc=");
          Serial.print(mqtt_client.state());
          delay(2000);
      }
  }
  mqtt_client.subscribe(mqtt_topic_self.c_str());
  Serial.print("Subscribed to topic: ");
  Serial.println(mqtt_topic_self.c_str());
  Serial.println("-- MQTT Strapped --");
}

void loop()
{
  // MQTT
  mqtt_client.loop();
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  // deseialize JSON
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);
  JsonObject json_obj = doc.as<JsonObject>();

  unsigned short message_idx = json_obj["message_idx"];

  Serial.print("Message index: ");
  Serial.println(message_idx);

  process_message(message_idx);

}

unsigned long epoch_get_time() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

// change screen and LEDs
void process_message(unsigned short message_idx) {
  // switch case
  switch(message_idx) {
    case 0: // Clear
      Serial.println("Received message 0");
      neopix->clear();
      FastLED.show();  // Update the pixel display
      break;
    case 1: // Rainbow
      Serial.println("Received message 1");
      neopix->rainbow();
      FastLED.show();  // Update the pixel display
      break;
    case 2: // Red
      Serial.println("Received message 2");
      neopix->pulse(CRGB::Red);
      FastLED.show();  // Update the pixel display
      break;
    case 3: // Green
      Serial.println("Received message 3");
      neopix->pulse(CRGB::Green);
      FastLED.show();  // Update the pixel display
      break;
    case 4: // Blue
      Serial.println("Received message 4");
      neopix->pulse(CRGB::Blue);
      FastLED.show();  // Update the pixel display
      break;
    default:
      Serial.println("Received unknown message");
      Serial.println(message_idx);
      process_message(0);
      break;
  }
}
