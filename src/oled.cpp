#include "OLED.hpp"

// Constructor
OLED::OLED() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        strapped = false;
        return;
    }
    // rotate display 180
    display.setRotation(2);
    display.setTextColor(WHITE);
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(0, 0);
    display.display();
    strapped = true;
}

// Destructor
OLED::~OLED() {
    display.clearDisplay();
    display.display();
}

void OLED::clear() {
    display.clearDisplay();
    display.display();
}

void OLED::update() {
    switch(current_state) {
        case 0: // Clear
            clear();
            break;
        case 1: // Hello
            display.println("TEST");
            display.display();
            break;
        default:
            clear();
            break;
    }
}