#ifndef PARSER_H
#define PARSER_H

// GUI
#define BUTTON_PIN 15 //  pin connected to button
#define FIRST_PAGE 1
#define PARAM_PAGE 2
#define OFFSET_PAGE 3
#define DELAY_PAGE 4
#define TEST_PAGE 5
#define ENDED_TEST_PAGE 6
#define ARROW_UP 10
#define ARROW_DOWN 20
#define OFFSET_ADDRESS 0
#define DELAY_ADDRESS 1
#define FILE_COUNTER_ADDRESS 2
#define GRAVITY 9.81
#define TEST_TIMER 600000 // 10 minutes testing

// Endereço do MPU6050 para o pino AD0 em baixa
#define MPU_ADDR 0x68
#define CALIBRATION_SIZE 100

// Limite de aceleração para objeto stopped
#define STOP_OFFSET 200
#define BUFFER_SIZE 10
#define EEPROM_SIZE 3

// 
#define GYRO_DEADZONE 1e-2
#define ACC_DEADZONE 1e-2

#define FILTER_ORDER 7
#define ARRAY_SIZE 50
#define TRESHOLD_VELOCITY 0.2 // treshold velocity (m/s)

typedef struct MPU {
    // Mean values
    float mean_acc_x;
    float mean_acc_y;
    float mean_acc_z;
    float mean_roll;
    float mean_pitch;
    float mean_yaw;

    // Raw data
    float raw_x[FILTER_ORDER] = {0};
    float raw_y[FILTER_ORDER] = {0};
    float raw_z[FILTER_ORDER] = {0};
    float raw_yaw[FILTER_ORDER] = {0};
    float raw_pitch[FILTER_ORDER] = {0};
    float raw_roll[FILTER_ORDER] = {0};

    // State variables
    float x[FILTER_ORDER] = {0};
    float y[FILTER_ORDER] = {0};
    float z[FILTER_ORDER] = {0};
    float yaw[FILTER_ORDER] = {0};
    float pitch[FILTER_ORDER] = {0};
    float roll[FILTER_ORDER] = {0};

    // Filtered values
    float acc_x = 0;
    float acc_y = 0;
    float acc_z = 0;
    float gyro_yaw = 0;
    float gyro_pitch = 0;
    float gyro_roll = 0;
} MPU;

typedef struct Message
{
	MPU data;
	int count;
} Message;

typedef struct Body {
    float pitch;
    float roll;
    float yaw;
    float max_z = 0;
    float min_z = 0;
    float delta_z;
    float x = 0;
    float y = 0;
    float z = 0;
    float vx = 0;
    float vy = 0;
    float vz = 0;
    float ax;
    float ay;
    float az;
    float ax_array[ARRAY_SIZE];
    float ay_array[ARRAY_SIZE];
    float az_array[ARRAY_SIZE];
    float vx_array[ARRAY_SIZE];
    float vy_array[ARRAY_SIZE];
    float vz_array[ARRAY_SIZE];
    int sample = 0;
} Body;
    
typedef struct Variaveis{
    Body body;
    MPU mpu;

    unsigned long t_samples;
    unsigned long t_stopped;
    bool stopped;
    float dist;
    int i;
    int counter = 0;
    volatile float aux_ac;
    float average;
    int offset_stop;
    float gravity_offset = 0;
    int delay_acc;

    int arrow; // this is the y position of selection sign ">"
    int page;	// page as default

    float timerMillisButton;
    float timerBackButton;

    int lastState; // the previous state from the input pin
    int currentState;	  // the current reading from the input pin
    
    int fileAddress; // enumeration of test file

    bool enterTestPage;
} Variables;

#endif