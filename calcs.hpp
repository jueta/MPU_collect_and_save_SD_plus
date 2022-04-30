#include <Arduino.h>
#include <MPU6050_light.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "definitions.hpp"

class BodyMovement {
    private: 
        Variables var;
        float apply_filter(float *x, float *y);
        float detrend(float *x);
        float B[FILTER_ORDER] = {
            0.2894,
            1.7364,
            4.3411,
            5.7881,
            4.3411,
            1.7364,
            0.2894
        };
        float A[FILTER_ORDER] = {
            1.0000,
            3.5794,
            5.6587,
            4.9654,
            2.5295,
            0.7053,
            0.0838
        };
        
    public:
        Variables *get_current_state();
        void eom(MPU6050 *mpu);
        void config_variables();
        void calibrate_imu(MPU6050 *mpu);
        void update_mpu(MPU6050 *mpu);
};
