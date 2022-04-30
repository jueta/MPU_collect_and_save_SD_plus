#include "sd.hpp"

// WeMos D1 esp8266: D8 as standard
const int chipSelect = SS;

extern Variables *var;
unsigned long testingTimer = 0;
extern QueueHandle_t msg_queue;
bool testFinished = false;

void sd_task(void *pvParameters)
{

	float t1, t2;
	Message rcv_msg;

	while (!Serial)
	{
		; // wait for serial port to connect. Needed for native USB port only
	}

	Serial.print("\nInitializing SD card...");

	// we'll use the initialization code from the utility libraries
	// since we're just testing if the card is working!

	vTaskDelay(pdMS_TO_TICKS(500));

	if (!SD.begin(SS))
	{
		Serial.println("initialization failed. Things to check:");
		Serial.println("* is a card inserted?");
		Serial.println("* is your wiring correct?");
		Serial.println("* did you change the chipSelect pin to match your shield or module?");
		esp_restart();
	}

	else
	{
		Serial.println("Wiring is correct and a card is present.");
	}

	// print the type of card
	Serial.println();
	Serial.print("Card type:         ");
	switch (SD.cardType())
	{
	case CARD_NONE:
		Serial.println("NONE");
		break;
	case CARD_MMC:
		Serial.println("MMC");
		break;
	case CARD_SD:
		Serial.println("SD");
		break;
	case CARD_SDHC:
		Serial.println("SDHC");
		break;
	default:
		Serial.println("Unknown");
	}

	Serial.print("Card size:  ");
	Serial.println((float)SD.cardSize() / 1000);

	Serial.print("Total bytes: ");
	Serial.println(SD.totalBytes());

	Serial.print("Used bytes: ");
	Serial.println(SD.usedBytes());

	// print the type and size of the first FAT-type volume
	//  uint32_t volumesize;
	//  Serial.print("Volume type is:    FAT");
	//  Serial.println(SDFS.usefatType(), DEC);

	uint64_t cardSize = SD.cardSize() / (1024 * 1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);

	listDir(SD, "/", 0);

	var->fileAddress += 1;
	EEPROM.write(FILE_COUNTER_ADDRESS, var->fileAddress);
	EEPROM.commit();

	char *str = "/test";
	char path[12];
	sprintf(path, "%s%d%s", "/test", var->fileAddress, ".txt");
	Serial.print("\n Test writing on file Address: ");
	Serial.println(path);
	char *tableColumns = "raw_x,raw_y,raw_z,raw_roll,raw_pitch,raw_yaw,acc_x,acc_y,acc_z";
	writeFile(SD, path, tableColumns);

	File file = SD.open(path, FILE_APPEND);
	if (!file)
	{
		Serial.println("Failed to open file for writing message");
		vTaskDelete(NULL);
	}
	else
	{

		// main loop of SD task
		for (;;)
		{

			if (xQueueReceive(msg_queue, (void *)&rcv_msg, 0) == pdTRUE)
			{
				Serial.print("\n Writing Values ");
				Serial.printf("Writing Message: %s\n", path);

				file.print("\n");
				file.print(rcv_msg.data.raw_x[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.raw_y[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.raw_z[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.raw_roll[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.raw_pitch[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.raw_yaw[FILTER_ORDER - 1]);
				file.print(",");
				file.print(rcv_msg.data.acc_x);
				file.print(",");
				file.print(rcv_msg.data.acc_y);
				file.print(",");
				file.print(rcv_msg.data.acc_z);
				// file.print("\nraw_x: ");
				// file.print(data.roll);
				// file.print("\nraw_x: ");
				// file.print(data.pitch);
				// file.print("\nraw_x: ");
				// file.print(data.yaw);

				if (millis() - testingTimer >= TEST_TIMER) // Test run for 10 minutes
				{
					Serial.print("\n Finish Testing! Killing task SD");
					file.close();
					testFinished = true;
					delay(100);
					vTaskDelete(NULL);
				};
			}
		}
	}
}

// --------------- FUNCTIONS ----------------------
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
	Serial.printf("Listing directory: %s\n", dirname);

	File root = fs.open(dirname);
	if (!root)
	{
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file)
	{
		if (file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels)
			{
				listDir(fs, file.name(), levels - 1);
			}
		}
		else
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
}

void createDir(fs::FS &fs, const char *path)
{
	Serial.printf("Creating Dir: %s\n", path);
	if (fs.mkdir(path))
	{
		Serial.println("Dir created");
	}
	else
	{
		Serial.println("mkdir failed");
	}
}

void removeDir(fs::FS &fs, const char *path)
{
	Serial.printf("Removing Dir: %s\n", path);
	if (fs.rmdir(path))
	{
		Serial.println("Dir removed");
	}
	else
	{
		Serial.println("rmdir failed");
	}
}

void readFile(fs::FS &fs, const char *path)
{
	Serial.printf("Reading file: %s\n", path);

	File file = fs.open(path);
	if (!file)
	{
		Serial.println("Failed to open file for reading");
		return;
	}

	Serial.print("Read from file: ");
	while (file.available())
	{
		Serial.write(file.read());
	}
	file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
	Serial.printf("Writing file: %s\n", path);
	Serial.printf("Message: %s\n", message);

	File file = fs.open(path, FILE_WRITE);
	if (!file)
	{
		Serial.println("Failed to open file for writing");
		return;
	}
	if (file.print(message))
	{
		Serial.println("File written");
	}
	else
	{
		Serial.println("Write failed");
	}
	file.close();
}


void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
	Serial.printf("Renaming file %s to %s\n", path1, path2);
	if (fs.rename(path1, path2))
	{
		Serial.println("File renamed");
	}
	else
	{
		Serial.println("Rename failed");
	}
}

void deleteFile(fs::FS &fs, const char *path)
{
	Serial.printf("Deleting file: %s\n", path);
	if (fs.remove(path))
	{
		Serial.println("File deleted");
	}
	else
	{
		Serial.println("Delete failed");
	}
}

void testFileIO(fs::FS &fs, const char *path)
{
	File file = fs.open(path);
	static uint8_t buf[512];
	size_t len = 0;
	uint32_t start = millis();
	uint32_t end = start;
	if (file)
	{
		len = file.size();
		size_t flen = len;
		start = millis();
		while (len)
		{
			size_t toRead = len;
			if (toRead > 512)
			{
				toRead = 512;
			}
			file.read(buf, toRead);
			len -= toRead;
		}
		end = millis() - start;
		Serial.printf("%u bytes read for %u ms\n", flen, end);
		file.close();
	}
	else
	{
		Serial.println("Failed to open file for reading");
	}

	file = fs.open(path, FILE_WRITE);
	if (!file)
	{
		Serial.println("Failed to open file for writing");
		return;
	}

	size_t i;
	start = millis();
	for (i = 0; i < 2048; i++)
	{
		file.write(buf, 512);
	}
	end = millis() - start;
	Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
	file.close();
}
