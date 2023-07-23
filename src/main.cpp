/*
 * Love Lamps Version 2
 * NMACERI
 */

// EXTERNAL LIBRARIES
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>

// INTERNAL LIBRARIES
#include "private.hpp"  // Passwords, keys, server info, etc.
#include "neopix.hpp"
#include "oled.hpp"
#include "messages.hpp"

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
bool mqtt_send_pending = false;
unsigned short mqtt_message_idx = 0;  // TODO: Repair the state machine so we don't need this

// UNIX EPOCH TIME
const char* ntpServer = "pool.ntp.org";
unsigned long epoch_get_time();

// Setup button inputs
#define ALT_BUTTON_PIN  33
#define ENTER_BUTTON_PIN 32
#define ALT_BUTTON_THRESHOLD 1000
#define ENTER_BUTTON_THRESHOLD 1000
#define BUTTON_DEBOUNCE 200
unsigned long button_last_pressed = 0;
void button_handler(int button_pin);

// Drive Objects
NeoPix* neopix;
OLED* oled;

// OLED
bool update_oled = false;
bool clear_oled = false;

// TIMER
ESP32Timer ITimer(1);
bool IRAM_ATTR TimerHandler1(void * timerNo);

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

  // OLED
  Serial.println("-- Strapping OLED --");
  oled = new OLED();
  if(!oled->strapped) {
    Serial.println("Failed to strap OLED");
  }
  Serial.println("-- OLED Strapped --");

  // BUTTONS
  Serial.println("-- Strapping Buttons --");
  pinMode(ALT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(ALT_BUTTON_PIN, []{button_handler(ALT_BUTTON_PIN);}, FALLING);
  attachInterrupt(ENTER_BUTTON_PIN, []{button_handler(ENTER_BUTTON_PIN);}, FALLING);
  Serial.println("-- Buttons Strapped --");

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
  mqtt_client.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt_client.setCallback(mqtt_callback);
  while (!mqtt_client.connected()) {
      String client_id = "cs-";
      client_id += String(WiFi.macAddress());
      Serial.printf("MQTT ID: %s\n", client_id.c_str());
      if (mqtt_client.connect(client_id.c_str(), MQTT_USER, MQTT_PASS)) {
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

  // TIMER
  Serial.println("-- Strapping Timer --");
  ITimer.attachInterruptInterval(OLED_TIMEOUT_MS * 1000, TimerHandler1);
  Serial.println("-- Timer Strapped --");

  Serial.println("-- Setup Complete --");
  
}

void loop()
{
  // MQTT
  mqtt_client.loop();

  if(mqtt_send_pending) {
    if(mqtt_message_idx > 0 && mqtt_message_idx <= MSG_MAX_IDX) {
      // send message
      StaticJsonDocument<200> doc;
      doc["message_idx"] = mqtt_message_idx;
      char buffer[200];
      serializeJson(doc, buffer);
      mqtt_client.publish(mqtt_topic_target.c_str(), buffer);
      mqtt_send_pending = false;
      oled->set_state(100);
      update_oled = true;
    }
    else {
      // invalid message
      mqtt_send_pending = false;
      oled->set_state(101);
      update_oled = true;
    }
    mqtt_message_idx = 0;
  }

  // OLED
  if(update_oled) {
    // if timer is running, stop it
    ITimer.stopTimer();
    oled->update();
    update_oled = false;
    // update timer
    ITimer.restartTimer();
  }
  if(clear_oled) {
    oled->clear();
    clear_oled = false;
    // reset state
    oled->set_state(0);
  }
}

bool IRAM_ATTR TimerHandler1(void * timerNo) {
  clear_oled = true;
  return true;
}


void button_handler(int button_pin) {
  // debounce
  if((millis() - button_last_pressed) < BUTTON_DEBOUNCE) {
    return;
  }
  button_last_pressed = millis();
  // check if button is pressed
  if(button_pin == ENTER_BUTTON_PIN) {
    // send message
    mqtt_message_idx = oled->get_state();
    oled->set_state(99);
    mqtt_send_pending = true;
  }
  else if(button_pin == ALT_BUTTON_PIN) {
    // switch message to be sent
    oled->next_state();
  }
  update_oled = true;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // stop timer
  ITimer.stopTimer();
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload, length);
  JsonObject json_obj = doc.as<JsonObject>();

  unsigned short message_idx = json_obj["message_idx"];

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
  // Update OLED
  oled->set_state(message_idx);
  oled->update();
  update_oled = true;
  // switch case
  switch(message_idx) {
    case CLEAR_IDX:
      neopix->clear();
      FastLED.show();  // Update the pixel display
      break;
    case ILY_IDX:
      neopix->pulse(CRGB::Red);
      break;
    case HELLO_IDX:
      neopix->rainbow();
      break;
    case GN_IDX: 
      neopix->pulse(CRGB::DarkSlateBlue);
      break;
    case GM_IDX:
      neopix->visor(CRGB::MistyRose);
      break;
    case IMY_IDX:
      neopix->chase(CRGB::Purple);
      break;
    case IH_IDX:
      neopix->pulse(CRGB::Amethyst);
      break;
    default:
      process_message(0);
      break;
  }
}

