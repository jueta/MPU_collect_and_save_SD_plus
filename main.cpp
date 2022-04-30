#include <Arduino.h>
#include <MPU6050_light.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "Wire.h"

#include "webServer.hpp"
#include "gui.hpp"
#include "main_task.hpp"
#include "sd.hpp"

float testMillisTimer;

TaskHandle_t ButtonTask;
TaskHandle_t MainTask;
TaskHandle_t DisplayTask;
TaskHandle_t WebServerTask;
TaskHandle_t SDcardTask;

// Classes
Variables *var;

QueueHandle_t msg_queue;
static const int msg_queue_len = 100;     // Size of msg_queue

// ----- main setup -----
void setup()
{
	Serial.begin(115200);
	Wire.begin();
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	msg_queue = xQueueCreate(msg_queue_len, sizeof(Message));

	// -----  Creating Tasks  ---------
	xTaskCreatePinnedToCore(&main_task, "Main Task", 10000, NULL, configMAX_PRIORITIES, &MainTask, 0);
	delay(500);

	xTaskCreatePinnedToCore(&sd_task, "SD Card Task", 10000, NULL, configMAX_PRIORITIES - 1, &SDcardTask, 1);
	delay(500);

	// xTaskCreatePinnedToCore( display_task, "Display Task", 10000, NULL, 2, &DisplayTask, 1);
	// delay(500);

	// xTaskCreatePinnedToCore( &web_task, "Web Server Task", 10000, NULL, 3, &WebServerTask, 1);
	// delay(500);
}

// ----- main loop -----
void loop()
{
	vTaskDelay(1000);
}
