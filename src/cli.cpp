#include "cli.hpp"

// Constructor
CLI::CLI() {
    // Initialize your commands here
    this->config = cli.addCmd("config");
    this->config.addPosArg("ssid");
    this->config.addPosArg("password");
    this->config.addPosArg("target");
    this->config.addPosArg("board_id");
    this->config.addPosArg("mqtt_server");
    this->config.addPosArg("mqtt_port");
    this->config.addPosArg("mqtt_user");
    this->config.addPosArg("mqtt_password");
    this->reset = cli.addCmd("reset");
    this->toggle_debug = cli.addCmd("toggle_debug");
    this->board_info = cli.addCmd("board_info");
    this->clear_nvs = cli.addCmd("clear_nvs");
}

// Destructor
CLI::~CLI() {
    // Clean up if necessary
}

void CLI::process_commands() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');

        if (input.length() > 0) {
            Serial.print("# ");
            Serial.println(input);

            cli.parse(input);
        }
    }

    if (cli.available()) {
        Command c = cli.getCmd();

        int argNum = c.countArgs();

        Serial.print("> ");
        Serial.print(c.getName());
        Serial.print(' ');

        for (int i = 0; i<argNum; ++i) {
            Argument arg = c.getArgument(i);
            Serial.print(arg.toString());
            Serial.print(' ');
        }

        Serial.println();

        if (c == config) {
            // Get the arguments
            String ssid = c.getArg("ssid").getValue();
            String password = c.getArg("password").getValue();
            String target = c.getArg("target").getValue();
            String board_id = c.getArg("board_id").getValue();
            String mqtt_server = c.getArg("mqtt_server").getValue();
            int mqtt_port = c.getArg("mqtt_port").getValue().toInt();
            String mqtt_user = c.getArg("mqtt_user").getValue();
            String mqtt_password = c.getArg("mqtt_password").getValue();
            configCommand(ssid, password, target, board_id, mqtt_server, mqtt_port, mqtt_user, mqtt_password);
        }
        else if (c == reset) {
            resetCommand();
        }
        else if (c == toggle_debug) {
            toggleDebugCommand();
        }
        else if (c == board_info) {
            boardInfoCommand();
        }
        else if (c == clear_nvs) {
            clearNVSCommand();
        }
    }

    if (cli.errored()) {
        CommandError cmdError = cli.getError();
        Log.errorln("%s", cmdError.toString().c_str());

        if (cmdError.hasCommand()) {
            Log.errorln("Did you mean \"%s\"?", cmdError.getCommand().toString().c_str());
        }
    }
}


void CLI::configCommand(String ssid, String password, String target, String board_id, String mqtt_server, int mqtt_port, String mqtt_user, String mqtt_password) {
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init(); // initialize the NVS partition.

    preferences.begin("love_lamps", false);
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_pass", password);
    preferences.putString("board_target", target);
    preferences.putString("board_id", board_id);
    preferences.putString("mqtt_server", mqtt_server);
    preferences.putInt("mqtt_port", mqtt_port);
    preferences.putString("mqtt_user", mqtt_user);
    preferences.putString("mqtt_password", mqtt_password);
    preferences.end();

    preferences.begin("love_lamps", true);
    // check that it was stored
    if(preferences.getString("wifi_ssid") != ssid) {
        preferences.end();
        Log.warningln("Failed to store preferences\nResetting...");
        ESP.restart();
    }
    else {
      preferences.end();
      Log.warningln("Preferences stored");
    }
}

void CLI::resetCommand() {
    Log.warningln("Resetting...");
    ESP.restart();
}

void CLI::toggleDebugCommand() {
    preferences.begin("love_lamps", false);
    preferences.putBool("debug", !preferences.getBool("debug", false));
    Log.warningln("You must restart the device for this to take effect");
    Log.warningln("Debug mode set to: %d", preferences.getBool("debug", false));
    preferences.end();
}

void CLI::boardInfoCommand() {
    Log.noticeln("MAC: %s", WiFi.macAddress().c_str());
    Log.noticeln("Serial: %s", String(ESP.getEfuseMac(), HEX).c_str());
    Log.noticeln("LL Version: v%d.%d.%d", VER_MAJOR, VER_MINOR, VER_PATCH);
    Log.warningln("BOARD_INFO_END");
}

void CLI::clearNVSCommand() {
     nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init(); // initialize the NVS partition.
    preferences.begin("love_lamps", false);
    preferences.putBool("debug", false);
    preferences.end();
    Log.warningln("NVS Cleared");
    ESP.restart();
}
