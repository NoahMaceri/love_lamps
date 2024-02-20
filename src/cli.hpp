#ifndef CLI_HPP
#define CLI_HPP

#include <Arduino.h>
#include <SimpleCLI.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <ArduinoLog.h>
#include <WiFi.h>

#include "ver.h"

class CLI {

public:
    // Functions
    CLI();
    ~CLI();

    void process_commands();
    void clearNVSCommand();
    
private:
    Preferences preferences;
    SimpleCLI cli;
    Command config;
    Command reset;
    Command toggle_debug;
    Command board_info;
    Command clear_nvs;

    void configCommand(String ssid, String password, String target, String board_id);
    void resetCommand();
    void toggleDebugCommand();
    void boardInfoCommand();
   
};

#endif // CLI_HPP