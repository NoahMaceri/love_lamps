#ifndef OLED_HPP
#define OLED_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C


class OLED {

public:
    // Functions
    OLED();
    ~OLED();
    void update();

    // Variables
    bool strapped = false;
    unsigned short current_state = 0;
    
private:
    // Functions
    void clear();

    // Variables
    Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
    
};


#endif // OLED_HPP