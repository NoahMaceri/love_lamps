#include "neopix.hpp"

// Constructor
NeoPix::NeoPix() {
    FastLED.addLeds<NEOPIX_LED_TYPE, NEOPIX_DATA_PIN, NEOPIX_COLOR_ORDER>(leds, NEOPIX_NUM_LEDS).setCorrection(TypicalLEDStrip).setDither(NEOPIX_BRIGHTNESS < 255);
    FastLED.setBrightness(NEOPIX_BRIGHTNESS);
    // Blank out the LEDS
    fadeToBlackBy(leds, NEOPIX_NUM_LEDS, 255);
    FastLED.show();  // Update the pixel display
    strapped = true;
}

// Destructor
NeoPix::~NeoPix() {
    // Blank out the LEDS
    fadeToBlackBy(leds, NEOPIX_NUM_LEDS, 255);
    FastLED.show();  // Update the pixel display
}

void NeoPix::clear()
{
  // slowly fade out 
  for(int i = 0; i < 255; i++) {
    for(int j = 0; j < NEOPIX_NUM_LEDS; j++) {
      leds[j].r -= 1;
      leds[j].g -= 1;
      leds[j].b -= 1;
    }
    FastLED.show();  // Update the pixel display
    delay(2);
  }
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void NeoPix::rainbow() 
{
  unsigned long time_out = millis() + NEOPIX_ANIM_LEN;
  while(millis() < time_out) {
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;
  
    uint8_t sat8 = beatsin88( 87, 220, 250);
    uint8_t brightdepth = beatsin88( 341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16;//gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);
    
    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis ;
    sLastMillis  = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88( 400, 5,9);
    uint16_t brightnesstheta16 = sPseudotime;
    
    for( uint16_t i = 0 ; i < NEOPIX_NUM_LEDS; i++) {
      hue16 += hueinc16;
      uint8_t hue8 = hue16 / 256;

      brightnesstheta16  += brightnessthetainc16;
      uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

      uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
      uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
      bri8 += (255 - brightdepth);
      
      CRGB newcolor = CHSV( hue8, sat8, bri8);
      
      uint16_t pixelnumber = i;
      pixelnumber = (NEOPIX_NUM_LEDS-1) - pixelnumber;
      
      nblend(leds[pixelnumber], newcolor, 64);
    }
    FastLED.show();  // Update the pixel display
  }
  // Clear all LEDs
  clear();
}

void NeoPix::pulse(CRGB color) {
    // slowly fade in and out the LEDs to and from the given color
    unsigned long time_out = millis() + NEOPIX_ANIM_LEN;

    // calculate steps for each color
    int red_step = color.r / NEOPIX_PULSE_STEP;
    int green_step = color.g / NEOPIX_PULSE_STEP;
    int blue_step = color.b / NEOPIX_PULSE_STEP;
    while(millis() < time_out) {
        // fade in
        for(int i = 0; i < 255; i++) {
            for(int j = 0; j < NEOPIX_NUM_LEDS; j++) {
                leds[j].r += red_step;
                leds[j].g += green_step;
                leds[j].b += blue_step;
            }
            FastLED.show();  // Update the pixel display
            delay(10);
        }
        // fade out
        for(int i = 0; i < 255; i++) {
            for(int j = 0; j < NEOPIX_NUM_LEDS; j++) {
                leds[j].r -= red_step;
                leds[j].g -= green_step;
                leds[j].b -= blue_step;
            }
            FastLED.show();  // Update the pixel display
            delay(10);
        }
    }
    clear();
}



