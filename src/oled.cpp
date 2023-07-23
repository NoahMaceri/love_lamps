#include "OLED.hpp"

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
    display.setTextSize(3);
    display.setCursor(0, 0);
    display.println("Welcome");
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
    display.clearDisplay();
    display.setTextSize(OLED_TEXT_SIZE);
    switch(current_state) {
        case CLEAR_IDX:
            break;
        case ILY_IDX:
            // calulate centering
            display.setCursor((OLED_WIDTH - (strlen(ILY_STR) * 12)) / 2, 0);
            display.println(ILY_STR);
            display.display();
            break;
        case HELLO_IDX:
            display.setCursor((OLED_WIDTH - (strlen(HELLO_STR) * 12)) / 2, 0);
            display.println(HELLO_STR);
            display.display();
            break;
        case GN_IDX:
            display.setCursor((OLED_WIDTH - (strlen(GN_STR) * 12)) / 2, 0);
            display.println(GN_STR);
            display.display();
            break;
        case GM_IDX:
            display.setCursor((OLED_WIDTH - (strlen(GM_STR) * 12)) / 2, 0);
            display.println(GM_STR);
            display.display();
            break;
        case IMY_IDX:
            display.setCursor((OLED_WIDTH - (strlen(IMY_STR) * 12)) / 2, 0);
            display.println(IMY_STR);
            display.display();
            break;
        case IH_IDX:
            display.setCursor((OLED_WIDTH - (strlen(IH_STR) * 12)) / 2, 0);
            display.println(IH_STR);
            display.display();
            break;
        case SENDING_IDX:
            display.setCursor((OLED_WIDTH - (strlen(SENDING_STR) * 12)) / 2, 0);
            display.println(SENDING_STR);
            display.display();
            break;
        case SENT_IDX:
            display.setCursor((OLED_WIDTH - (strlen(SENT_STR) * 12)) / 2, 0);
            display.println(SENT_STR);
            display.display();
            break;
        case NO_MSG_IDX:
            display.setCursor((OLED_WIDTH - (strlen(NO_MSG_STR) * 12)) / 2, 0);
            display.println(NO_MSG_STR);
            display.display();
            break;
        default:
            break;
    }
    display.display();
}

void OLED::set_state(unsigned short state) {
    current_state = state;
}

void OLED::next_state() {
    current_state++;
    if(current_state > MSG_MAX_IDX) {
        current_state = 1;
    }
}
