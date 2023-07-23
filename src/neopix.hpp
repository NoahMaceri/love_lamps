#ifndef NEOPIX_HPP
#define NEOPIX_HPP

#include <Arduino.h>
#include <FastLED.h>

// NEOPIXEL
#define NEOPIX_DATA_PIN     15
#define NEOPIX_LED_TYPE     WS2811
#define NEOPIX_COLOR_ORDER  GRB
#define NEOPIX_NUM_LEDS     12
#define NEOPIX_BRIGHTNESS   40
#define NEOPIX_ANIM_LEN     5000
#define NEOPIX_PULSE_DELAY  20
#define NEOPIX_CHASE_DELAY  20
#define NEOPIX_TRAIL_DELAY  50

class NeoPix {

public:
    // Constructor
    NeoPix();

    // Destructor
    ~NeoPix();

    bool strapped = false;

    void clear();
    void rainbow();
    void pulse(CRGB color);
    void chase(CRGB color);
    void visor(CRGB color);
private:
    CRGB leds[NEOPIX_NUM_LEDS];

};


#endif // NEOPIX_HPP