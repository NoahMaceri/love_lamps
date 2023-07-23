#ifndef OLED_HPP
#define OLED_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "messages.hpp"

#define SSD1306_NO_SPLASH // Free up a little memory

#define OLED_WIDTH      128
#define OLED_HEIGHT     32
#define OLED_RESET      -1
#define OLED_ADDRESS    0x3C
#define OLED_TIMEOUT_MS 5000
#define OLED_TEXT_SIZE  2


class OLED {

public:
    // Functions
    OLED();
    ~OLED();
    void update();
    void clear();
    void set_state(unsigned short state);
    void next_state();
    unsigned short get_state() { return current_state; }

    // Variables
    bool strapped = false;
    
private:
    // Functions

    // Variables
    Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
    unsigned short current_state = 0;
    
};


#endif // OLED_HPP