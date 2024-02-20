/*
 * Love Lamps - Revision 2
 * NMACERI
 */

/* INCLUDES */
// EXTERNAL
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP32TimerInterrupt.hpp>
#include <Preferences.h>
#include <nvs_flash.h>
#include <ArduinoLog.h>
#include <Arduino_ESP32_OTA.h>

// INTERNAL
#include "private.hpp"  // Passwords, keys, server info, etc.
#include "neopix.hpp"
#include "oled.hpp"
#include "messages.hpp"
#include "cli.hpp"
#include "ver.h"
#include "root_ca.hpp"

/* DEFINES */
// Buttons
#define ALT_BUTTON_PIN  33
#define ENTER_BUTTON_PIN 32
#define ALT_BUTTON_THRESHOLD 1000
#define ENTER_BUTTON_THRESHOLD 1000
#define BUTTON_DEBOUNCE 200

/* OBJECTS */
Preferences preferences;
NeoPix* neopix;
OLED* oled;
CLI* cli;
WiFiClient mqtt_wifi_driver;
PubSubClient mqtt_client(mqtt_wifi_driver);
ESP32Timer ITimer(1);

/* VARIABLES */
String mqtt_base_topic = "love_lamps";
String mqtt_topic_target;
String mqtt_topic_self;
String WIFI_SSID;
String WIFI_PASS;
String BOARD_ID;
String BOARD_TARGET;
String ota_url;
String ota_ver;
char output_buffer[254];
bool mqtt_send_pending = false;
bool mqtt_ack_received = false;
bool mqtt_ack_pending = false;
bool update_oled = false;
bool clear_oled = false;
bool debug_mode = true;
bool ota_pending = false;
uint8_t mqtt_message_idx = 0;  // TODO: Repair the state machine so we don't need this
uint8_t fail_counter = 0;
uint32_t sent_timestamp = 0;
uint32_t button_last_pressed = 0;

/* FUNCTIONS */
void process_message(uint8_t message_idx, uint32_t timestamp);
void send_ack_message(uint32_t timestamp);
void mqtt_callback(char* topic, byte* payload, uint16_t length);
void button_handler(int button_pin);
bool IRAM_ATTR TimerHandler1(void * timerNo);
void config_mode();
void process_commands();
void printPrefix(Print* _logOutput, int logLevel) {
  char timestamp[20];
  // format millis
  sprintf(timestamp, "[%06lu] ", millis());
  _logOutput->print(timestamp);
  switch (logLevel)
  {
      default:
      case 0:_logOutput->print("[SILENT] " ); break;
      case 1:_logOutput->print("[FATAL] "  ); break;
      case 2:_logOutput->print("[ERROR] "  ); break;
      case 3:_logOutput->print("[WARNING] "); break;
      case 4:_logOutput->print("[INFO] "   ); break;
      case 5:_logOutput->print("[TRACE] "  ); break;
      case 6:_logOutput->print("[DEBUG] "); break;
  }   
}

void setup() {
  // SERIAL 
  Serial.begin(115200);
  // CLI
  cli = new CLI();
  // LOGGING
  preferences.begin("love_lamps", false);
  Log.setPrefix(printPrefix); // set prefix similar to NLog
  // check that we have a valid preferences file
  debug_mode = preferences.getBool("debug");
  if(debug_mode) {
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  }
  else {
    Log.begin(LOG_LEVEL_NOTICE, &Serial);
  }
  Log.setShowLevel(false);    // Do not show loglevel, we will do this in the prefix

  if(!(preferences.isKey("wifi_ssid"))) {
    Log.errorln("INIT MODE");
    while(true) {
      cli->process_commands();
    }
  }

  Log.noticeln("-- Love Lamps v%d.%d.%d --", VER_MAJOR, VER_MINOR, VER_PATCH);

  // PREFERENCES
  Log.verboseln("Strapping Preferences");
  // Grab wifi credentials
  Log.verboseln("Found preferences");
  WIFI_SSID = preferences.getString("wifi_ssid");
  WIFI_PASS = preferences.getString("wifi_pass");
  BOARD_ID = preferences.getString("board_id");
  BOARD_TARGET = preferences.getString("board_target");
  debug_mode = preferences.getBool("debug");
  preferences.end();

  mqtt_topic_target = mqtt_base_topic + "/" + BOARD_TARGET;
  mqtt_topic_self = mqtt_base_topic + "/" + BOARD_ID;
  Log.verboseln("Preferences Strapped");
  
  // NEOPXIEL
  Log.verboseln("Strapping NeoPixel");
  neopix = new NeoPix();
  if(!neopix->strapped) {
    Log.verboseln("Failed to strap NeoPixel");
  }
  Log.verboseln("NeoPixel Strapped");

  // OLED
  Log.verboseln("Strapping OLED");
  oled = new OLED();
  if(!oled->strapped) {
    Log.verboseln("Failed to strap OLED");
  }
  Log.verboseln("OLED Strapped");

  // BUTTONS
  Log.verboseln("Strapping Buttons");
  pinMode(ALT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENTER_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(ALT_BUTTON_PIN, []{button_handler(ALT_BUTTON_PIN);}, FALLING);
  attachInterrupt(ENTER_BUTTON_PIN, []{button_handler(ENTER_BUTTON_PIN);}, FALLING);
  Log.verboseln("Buttons Strapped");

  // Wifi
  Log.verboseln("Strapping Wifi");
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
  Log.verboseln("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    fail_counter++;
    if(fail_counter > 10) {
      Log.verboseln("Failed to connect to WiFi");
      Log.verboseln("Resetting...");
      cli->clearNVSCommand();
    }
  }
  Log.verboseln("Connected to the WiFi network with IP Address: %s", WiFi.localIP().toString().c_str());
  Log.verboseln("Wifi Strapped");
  fail_counter = 0;

  // MQTT
  Log.verboseln("Strapping MQTT");
  mqtt_client.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt_client.setCallback(mqtt_callback);
  while (!mqtt_client.connected()) {
    String client_id = "cs-";
    client_id += String(WiFi.macAddress());
    Log.verboseln("MQTT ID: %s", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str(), MQTT_USER, MQTT_PASS)) {
        Log.verboseln("Conneceted to MQTT broker");
    } else {
      Log.verboseln("Failed to connect, rc=%s", mqtt_client.state());
      delay(2000);
      fail_counter++;
      if(fail_counter > 5) {
        Log.verboseln("Failed to connect to MQTT");
        Log.verboseln("Resetting...");
        ESP.restart();
      }
    }
  }
  mqtt_client.subscribe(mqtt_topic_self.c_str());
  Log.verboseln("Subscribed to topic: %s", mqtt_topic_self.c_str());
  Log.verboseln("MQTT Strapped");

  // TIMER
  Log.verboseln("Strapping Timer");
  ITimer.attachInterruptInterval(OLED_TIMEOUT_MS * 1000, TimerHandler1);
  Log.verboseln("Timer Strapped");

  Log.noticeln("Setup Complete");
}

void loop() {
  // CLI
  cli->process_commands();
  // MQTT
  mqtt_client.loop();

  if(mqtt_ack_received) {
    oled->set_state(SENT_IDX, true);
    update_oled = true;
    mqtt_ack_received = false;
    mqtt_ack_pending = false;
  }

  if(mqtt_send_pending && mqtt_message_idx != 0) {
    // send message
    JsonDocument doc;
    doc["type"] = MAIN_MSG_TYPE;
    sent_timestamp = millis();
    doc["timestamp"] = sent_timestamp;
    doc["message_idx"] = mqtt_message_idx;
    // clear output buffer
    memset(output_buffer, 0, sizeof(output_buffer));
    serializeJson(doc, output_buffer);
    mqtt_client.publish(mqtt_topic_target.c_str(), output_buffer);
    mqtt_send_pending = false;
    oled->set_state(SENDING_IDX, true);
    update_oled = true;
    mqtt_ack_pending = true;
    mqtt_message_idx = 0;
  } else if(mqtt_send_pending && mqtt_message_idx == 0) {
    // no message selected
    oled->set_state(NO_MSG_IDX, true);
    update_oled = true;
    mqtt_send_pending = false;
  }

  // OLED
  if(clear_oled) {
    oled->clear();
    clear_oled = false;
    // reset state
    oled->set_state(CLEAR_IDX);
  }

  if(update_oled) {
    ITimer.stopTimer();
    oled->update();
    update_oled = false;
    ITimer.restartTimer();
  }
  
  // Check for MQTT server dicsonnect
  if(!mqtt_client.connected()) {
    // Reconnect
    Log.verboseln("MQTT Disconnected");
    Log.verboseln("Reconnecting...");
    while (!mqtt_client.connected()) {
      String client_id = "cs-";
      client_id += String(WiFi.macAddress());
      Log.verboseln("MQTT ID: %s", client_id.c_str());
      if (mqtt_client.connect(client_id.c_str(), MQTT_USER, MQTT_PASS)) {
          Log.verboseln("Conneceted to MQTT broker");
      } else {
        Log.verboseln("Failed to connect, rc=%s", mqtt_client.state());
        delay(2000);
        fail_counter++;
        if(fail_counter > 50) {
          Log.verboseln("Failed to connect to MQTT");
          Log.verboseln("Resetting...");
          ESP.restart();
        }
      }
    }
  }

  // OTA
  if(ota_pending) {
    Log.verboseln("Starting OTA update");
    Arduino_ESP32_OTA ota;
    Arduino_ESP32_OTA::Error ota_err = Arduino_ESP32_OTA::Error::None;
    ota.setCACert(root_ca);
    Serial.println("Initializing OTA storage");
    if ((ota_err = ota.begin()) != Arduino_ESP32_OTA::Error::None)
    {
      Serial.print  ("Arduino_ESP_OTA::begin() failed with error code ");
      Serial.println((int)ota_err);
      ota_pending = false;
      ESP.restart();
    }

    Serial.println("Starting download to flash ...");
    int const ota_download = ota.download(ota_url.c_str());
    if (ota_download <= 0)
    {
      Serial.print  ("Arduino_ESP_OTA::download failed with error code ");
      Serial.println(ota_download);
      ota_pending = false;
      ESP.restart();
    }
    Serial.print  (ota_download);
    Serial.println(" bytes stored.");

    Serial.println("Verify update integrity and apply ...");
    if ((ota_err = ota.update()) != Arduino_ESP32_OTA::Error::None)
    {
      Serial.print  ("ota.update() failed with error code ");
      Serial.println((int)ota_err);
      ota_pending = false;
      ESP.restart();
    }

    Serial.println("OTA update complete");
    ota.reset();
  }

}

bool IRAM_ATTR TimerHandler1(void * timerNo) {
  // Check if we were waiting for an ACK
  if(mqtt_ack_pending) {
    oled->set_state(FAILED_IDX, true);
    update_oled = true;
    mqtt_ack_pending = false;
    mqtt_ack_received = false;
    sent_timestamp = 0;
    // reset timer
    ITimer.restartTimer();
  } else {
    clear_oled = true;
  }
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
    mqtt_send_pending = true;
  } else if(button_pin == ALT_BUTTON_PIN) {
    // switch message to be sent
    oled->next_state();
  }
  update_oled = true;
}

void mqtt_callback(char* topic, byte* payload, uint16_t length) {
  // stop timer
  ITimer.stopTimer();
  JsonDocument doc;
  deserializeJson(doc, payload, length);
  JsonObject json_obj = doc.as<JsonObject>();

  uint8_t msg_type = json_obj["type"];
  uint32_t timestamp = json_obj["timestamp"];
  Log.verboseln("Message %d received at %d", msg_type, timestamp);
  switch(msg_type) {
    case MAIN_MSG_TYPE: {
      uint8_t message_idx = json_obj["message_idx"];
      process_message(message_idx, timestamp);
      break;
    }
    case ACK_MSG_TYPE: {
      if(timestamp == sent_timestamp) {
        mqtt_ack_pending = false;
        mqtt_ack_received = true;
        sent_timestamp = 0;
      }
      Log.verboseln("ACK received");
      break;
    }
    case OTA_MSG_TYPE: {
      Log.verboseln("Beginning OTA update");
      ota_url = json_obj["url"].as<String>();
      ota_ver = json_obj["version"].as<String>();
      String ver_str = "v" + String(VER_MAJOR) + "." + String(VER_MINOR) + "." + String(VER_PATCH);
      if(ota_ver != ver_str) {
        ota_pending = true;
      } else {
        Log.verboseln("Already on latest version");
      }
      break;
    }
    default:
      break;
  }
}

void send_ack_message(uint32_t timestamp) {
  JsonDocument doc;
  doc["type"] = ACK_MSG_TYPE;
  doc["timestamp"] = timestamp;
  // clear output buffer
  memset(output_buffer, 0, sizeof(output_buffer));
  serializeJson(doc, output_buffer);
  mqtt_client.publish(mqtt_topic_target.c_str(), output_buffer);
}

// change screen and LEDs
void process_message(uint8_t message_idx, uint32_t timestamp) {
  // Update OLED
  oled->set_state(message_idx, true);
  oled->update();
  oled->anim_trig = true;  // Force an animation after the LEDs trigger
  update_oled = true;
    // Send ACK
  send_ack_message(timestamp);
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
    case CALL_IDX:
      neopix->visor(CRGB::DarkOrange);
      break;
    case POG_IDX:
      neopix->crazy_rainbow();
      break;
    default:
      process_message(0, timestamp);
      break;
  }
}
