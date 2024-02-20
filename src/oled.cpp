#include "OLED.hpp"
#include "ver.h"

// Animations
#include "animations/sun_anim.h"
#include "animations/heart_anim.h"
#include "animations/wave_anim.h"
#include "animations/moon_anim.h"
#include "animations/friend_anim.h"
#include "animations/cook_anim.h"
#include "animations/call_anim.h"
#include "animations/rocket_anim.h"
#include "animations/check_anim.h"
#include "animations/letter_x_anim.h"

// Constructor
OLED::OLED() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("SSD1306 allocation failed");
        strapped = false;
        return;
    }
    // Init display
    display.setTextColor(SSD1306_WHITE);

    // Clear the buffer
    display.clearDisplay();
    
    // Display welcome message
    display.setTextSize(OLED_TEXT_SIZE);
    char* version = new char[7];
    sprintf(version, "v%d.%d.%d", VER_MAJOR, VER_MINOR, VER_PATCH);
    display.setCursor((OLED_WIDTH - (strlen("Love Lamps") * 12)) / 2, 0);
    display.println("Love Lamps");
    display.setCursor((OLED_WIDTH - (strlen(version) * 12)) / 2, 16);
    display.println(version);
    display.display();
    strapped = true;
}

// Destructor
OLED::~OLED() {
    display.clearDisplay();
    display.display();
    strapped = false;
}

void OLED::clear() {
    display.clearDisplay();
    display.display();
}

void OLED::update() {
    // Redirect for animation
    if (anim_trig) {
        update_anim();
        anim_trig = false;
        return;
    }
    display.clearDisplay();
    display.setTextSize(OLED_TEXT_SIZE);
    switch(current_state) {
        case CLEAR_IDX:
            break;
        case ILY_IDX:
            // calulate centering for width and height
            display.setCursor((OLED_WIDTH - (strlen(ILY_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(ILY_STR);
            display.display();
            break;
        case HELLO_IDX:
            display.setCursor((OLED_WIDTH - (strlen(HELLO_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(HELLO_STR);
            display.display();
            break;
        case GN_IDX:
            display.setCursor((OLED_WIDTH - (strlen(GN_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(GN_STR);
            display.display();
            break;
        case GM_IDX:
            display.setCursor((OLED_WIDTH - (strlen(GM_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(GM_STR);
            display.display();
            break;
        case IMY_IDX:
            display.setCursor((OLED_WIDTH - (strlen(IMY_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(IMY_STR);
            display.display();
            break;
        case IH_IDX:
            display.setCursor((OLED_WIDTH - (strlen(IH_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(IH_STR);
            display.display();
            break;
        case CALL_IDX:
            display.setCursor((OLED_WIDTH - (strlen(CALL_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(CALL_STR);
            display.display();
            break;
        case POG_IDX:
            display.setCursor((OLED_WIDTH - (strlen(POG_STR) * 12)) / 2, (OLED_HEIGHT - 12) / 2);
            display.println(POG_STR);
            display.display();
            break;
        default:
            break;
    }
}

void OLED::update_anim() {
    display.clearDisplay();
    display.setTextSize(OLED_TEXT_SIZE);
    switch(current_state) {
        case CLEAR_IDX:
            break;
        case ILY_IDX:
            for(uint8_t i = 0; i < heart_allArray_LEN; i++) {
                // 64 x 64 bitmap
                display.drawBitmap(0, 0, heart_allArray[i], 32, 32, SSD1306_WHITE);
                // put text next to sun (two lines)
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("I love") * 12)) / 2, 0);
                display.println("I love");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("you <3") * 12)) / 2, 16);
                display.println("you <3");
                display.display();
                display.clearDisplay();
                // check if we are on the last frame
                if(i != heart_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case HELLO_IDX:
            for(uint8_t i = 0; i < wave_allArray_LEN; i++) {
                // 64 x 64 bitmap
                display.drawBitmap(0, 0, wave_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Hello!") * 12)) / 2, (OLED_HEIGHT - 12) / 2);
                display.println("Hello!");
                display.display();
                display.clearDisplay();
                if(i != wave_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case GN_IDX:
            for(uint8_t i = 0; i < moon_allArray_LEN; i++) {
                // 64 x 64 bitmap
                display.drawBitmap(0, 0, moon_allArray[i], 32, 32, SSD1306_WHITE);
                // put text next to sun (two lines)
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Good") * 12)) / 2, 0);
                display.println("Good");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Night") * 12)) / 2, 16);
                display.println("Night");
                display.display();
                display.clearDisplay();
                // check if we are on the last frame
                if(i != moon_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case GM_IDX:
            for(uint8_t i = 0; i < sun_allArray_LEN; i++) {
                display.drawBitmap(0, 0, sun_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Good") * 12)) / 2, 0);
                display.println("Good");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Morning") * 12)) / 2, 16);
                display.println("Morning");
                display.display();
                if(i != sun_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case IMY_IDX:
            for(uint8_t i = 0; i < friend_allArray_LEN; i++) {
                display.drawBitmap(0, 0, friend_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("I miss") * 12)) / 2, 0);
                display.println("I miss");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("you <3") * 12)) / 2, 16);
                display.println("you <3");
                display.display();
                if(i != friend_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case IH_IDX:
            for(uint8_t i = 0; i < cook_allArray_LEN; i++) {
                display.drawBitmap(0, 0, cook_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("I am") * 12)) / 2, 0);
                display.println("I am");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("hungry!") * 12)) / 2, 16);
                display.println("hungry!");
                display.display();
                if(i != cook_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case CALL_IDX:
            for(uint8_t i = 0; i < call_allArray_LEN; i++) {
                display.drawBitmap(0, 0, call_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Call") * 12)) / 2, 0);
                display.println("Call");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("me!") * 12)) / 2, 16);
                display.println("me!");
                display.display();
                if(i != call_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case POG_IDX:
            for(uint8_t i = 0; i < rocket_allArray_LEN; i++) {
                display.drawBitmap(0, 0, rocket_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("POG") * 12)) / 2, (OLED_HEIGHT - 12) / 2);
                display.println("POG");
                display.display();
                if(i != rocket_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case SENT_IDX:
            for(uint8_t i = 0; i < check_allArray_LEN; i++) {
                display.drawBitmap(0, 0, check_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Sent!") * 12)) / 2, (OLED_HEIGHT - 12) / 2);
                display.println("Sent!");
                display.display();
                if(i != check_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case SENDING_IDX:
            for(uint8_t i = 0; i < rocket_allArray_LEN; i++) {
                display.drawBitmap(0, 0, rocket_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Sending") * 12)) / 2, (OLED_HEIGHT - 12) / 2);
                display.println("Sending");
                display.display();
                if(i != rocket_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case FAILED_IDX:
            for(uint8_t i = 0; i < letter_x_allArray_LEN; i++) {
                display.drawBitmap(0, 0, letter_x_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Failed") * 12)) / 2, 0);
                display.println("Failed");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("to send!") * 12)) / 2, 16);
                display.println("to send!");
                display.display();
                if(i != letter_x_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        case NO_MSG_IDX:
            for(uint8_t i = 0; i < letter_x_allArray_LEN; i++) {
                display.drawBitmap(0, 0, letter_x_allArray[i], 32, 32, SSD1306_WHITE);
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("Select") * 12)) / 2, 0);
                display.println("Select");
                display.setCursor(40 + ((OLED_WIDTH - 40) - (strlen("message") * 12)) / 2, 16);
                display.println("message");
                display.display();
                if(i != letter_x_allArray_LEN - 1) {
                    display.clearDisplay();
                }
                delay(OLED_ANIM_DELAY);
            }
            break;
        default:
            break;
    }
}

void OLED::set_state(uint8_t state, bool anim) {
    anim_trig = anim;
    current_state = state;
}

void OLED::next_state() {
    current_state++;
    if(current_state > MSG_MAX_IDX) {
        current_state = 1;
    }
}
