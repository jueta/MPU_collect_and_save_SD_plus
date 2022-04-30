#include <Arduino.h>
#include "definitions.hpp"
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <EEPROM.h>

void display_task(void *pvParameters);

// ----- GUI functions -----
class GUI {
    public:
        void arrow_mov(Variables *var);
        void button_logic(Variables *var);
        void page_ui(Variables *var, Adafruit_SSD1306 *display);
        void regressive_counter_seconds(int numberOfCounter, Adafruit_SSD1306 *display);
};
