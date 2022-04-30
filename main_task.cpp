#include "main_task.hpp"
#include "calcs.hpp"

extern Variables *var;
extern long testingTimer;
extern QueueHandle_t msg_queue;
extern bool testFinished;

void main_task(void *pvParameters)
{

	Serial.print("main_task running on core ");
	Serial.println(xPortGetCoreID());

	BodyMovement calcs;
	MPU6050 mpu(Wire);
	Message rcv_msg;

	float t1, t2;

	// ----- CONFIG VARIABLES -----
	var = calcs.get_current_state();
	calcs.config_variables();

	// ----- EEPROM config -----
	config_eeprom(var);

	// -----MPU Setup -----
	byte status = mpu.begin(3, 3);
	Serial.print(F("MPU6050 status: "));
	Serial.println(status);
	while (status != 0)
	{
	} // stop everything if could not connect to MPU6050

	Serial.println(F("Calibrating IMU, do not move MPU6050"));
	calcs.calibrate_imu(&mpu);
	Serial.println("Done!\n");

	// -----  Sampling config  ---------
	var->t_samples = micros();

	for (;;)
	{

		t1 = millis();
		mpu.update();

		// Perform all computations
		calcs.update_mpu(&mpu);

		rcv_msg.data = var->mpu;

		// Send to other task via queue
		if (xQueueSend(msg_queue, (void *)&rcv_msg, 10) != pdTRUE)
		{
			Serial.println("ERROR: Could not put item on queue.");
		}
		else
		{
			Serial.println("\nMessage sent to queue.");
		}

		// // Raw MPU
		// std::cout << var->mpu.raw_x[FILTER_ORDER - 1] << " " << var->mpu.raw_y[FILTER_ORDER - 1] << " " << var->mpu.raw_z[FILTER_ORDER - 1] << std::endl;
		// std::cout << var->mpu.raw_roll[FILTER_ORDER - 1] << " " << var->mpu.raw_pitch[FILTER_ORDER - 1] << " " << var->mpu.raw_yaw[FILTER_ORDER - 1] << std::endl;

		// // Filtered MPU
		// std::cout << var->mpu.acc_x << " " << var->mpu.acc_y << " " << var->mpu.acc_z << std::endl;
		// std::cout << var->mpu.roll << " " << var->mpu.pitch << " " << var->mpu.yaw << std::endl;

		if(testFinished){
			Serial.print("\n Finish Testing! Killing task MPU");
			vTaskDelete(NULL);
		}

		t2 = millis() - t1;
		vTaskDelay(pdMS_TO_TICKS(var->delay_acc > t2 ? var->delay_acc - t2 : 0));
	}
}

// configurations
void config_eeprom(Variables *var)
{
	EEPROM.begin(EEPROM_SIZE);
	if (EEPROM.read(OFFSET_ADDRESS) == 255 || EEPROM.read(DELAY_ADDRESS) == 255 || EEPROM.read(FILE_COUNTER_ADDRESS) == 255)
	{
		EEPROM.write(OFFSET_ADDRESS, 3);
		EEPROM.write(DELAY_ADDRESS, 10);
		EEPROM.write(FILE_COUNTER_ADDRESS, 0);
	}
	var->offset_stop = EEPROM.read(OFFSET_ADDRESS);
	var->delay_acc = EEPROM.read(DELAY_ADDRESS);
	var->fileAddress = EEPROM.read(FILE_COUNTER_ADDRESS);

	Serial.print("offset_stop: ");
	Serial.println(var->offset_stop);
	Serial.print("delay_acc: ");
	Serial.println(var->delay_acc);
	Serial.print("Test Number: ");
	Serial.println(var->fileAddress);
}