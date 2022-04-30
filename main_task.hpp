#include <Arduino.h>
#include <MPU6050_light.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <EEPROM.h>
#include "definitions.hpp"

void main_task(void *pvParameters);

void config_eeprom(Variables *var);