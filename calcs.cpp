#include "calcs.hpp"

void BodyMovement::update_mpu(MPU6050 *mpu) {
    float x, y, z, yaw, pitch, roll;

    // READING PHASE
    // Update mpu
    x = mpu->getAccX();
    y = mpu->getAccY();
    z = mpu->getAccZ();
    roll = mpu->getAngleY();
    pitch = mpu->getAngleX();
    yaw = mpu->getAngleZ();
    
    // Check for deadzone
    x = abs(x) < ACC_DEADZONE ? 0 : x;
    y = abs(y) < ACC_DEADZONE ? 0 : y;
    z = abs(z) < ACC_DEADZONE ? 0 : z;

    yaw = abs(yaw) < GYRO_DEADZONE ? 0 : yaw;
    pitch = abs(pitch) < GYRO_DEADZONE ? 0 : pitch;
    roll = abs(roll) < GYRO_DEADZONE ? 0 : roll;

    // FILTER PHASE
    for (int i = 0; i < ARRAY_SIZE - 1; i++) {
        if (i < FILTER_ORDER) {
            // Shidt the raw array
            var.mpu.raw_x[i] = var.mpu.raw_x[i + 1];
            var.mpu.raw_y[i] = var.mpu.raw_y[i + 1];
            var.mpu.raw_z[i] = var.mpu.raw_z[i + 1];
            var.mpu.raw_yaw[i] = var.mpu.raw_yaw[i + 1];
            var.mpu.raw_pitch[i] = var.mpu.raw_pitch[i + 1];

            // Shift the output array
            var.mpu.x[i] = var.mpu.x[i + 1];
            var.mpu.y[i] = var.mpu.y[i + 1];
            var.mpu.z[i] = var.mpu.z[i + 1];
            var.mpu.yaw[i] = var.mpu.yaw[i + 1];
            var.mpu.pitch[i] = var.mpu.pitch[i + 1];
            var.mpu.roll[i] = var.mpu.roll[i + 1];
        }

        var.body.ax_array[i] = var.body.ax_array[i + 1];
        var.body.ay_array[i] = var.body.ay_array[i + 1];
        var.body.az_array[i] = var.body.az_array[i + 1];
    }

    // Add the new values
    var.mpu.raw_x[FILTER_ORDER - 1] = x;
    var.mpu.raw_y[FILTER_ORDER - 1] = y;
    var.mpu.raw_z[FILTER_ORDER - 1] = z;
    var.mpu.raw_yaw[FILTER_ORDER - 1] = yaw;
    var.mpu.raw_pitch[FILTER_ORDER - 1] = pitch;
    var.mpu.raw_roll[FILTER_ORDER - 1] = roll;

    // Apply filter
    var.mpu.x[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_x, var.mpu.x);
    var.mpu.y[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_y, var.mpu.y);
    var.mpu.z[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_z, var.mpu.z);
    var.mpu.yaw[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_yaw, var.mpu.yaw);
    var.mpu.pitch[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_pitch, var.mpu.pitch);
    var.mpu.roll[FILTER_ORDER - 1] = apply_filter(var.mpu.raw_roll, var.mpu.roll);

    // Update last values
    var.mpu.acc_x = var.mpu.x[FILTER_ORDER - 1];
    var.mpu.acc_y = var.mpu.y[FILTER_ORDER - 1];
    var.mpu.acc_z = var.mpu.z[FILTER_ORDER - 1];
    var.mpu.gyro_yaw = var.mpu.yaw[FILTER_ORDER - 1];
    var.mpu.gyro_pitch = var.mpu.pitch[FILTER_ORDER - 1];
    var.mpu.gyro_roll = var.mpu.roll[FILTER_ORDER - 1];

    var.body.ax = var.mpu.acc_x;
    var.body.ay = var.mpu.acc_y;
    var.body.az = var.mpu.acc_z;

    // Multiply by gravity
    var.body.az *= GRAVITY;
    var.body.ax *= GRAVITY;
    var.body.ay *= GRAVITY;
    
    // Save in the acc array 
    var.body.ax_array[ARRAY_SIZE - 1] = var.body.ax;
    var.body.ay_array[ARRAY_SIZE - 1] = var.body.ay;
    var.body.az_array[ARRAY_SIZE - 1] = var.body.az;

}

Variables *BodyMovement::get_current_state(){
    Variables *out;
    out = &var;
    return out;
}

void BodyMovement::eom(MPU6050 *mpu) {
        float ax_old, ay_old, az_old;
        float ax_new, ay_new, az_new;
        float vx_old, vy_old, vz_old;

        ax_old = detrend(var.body.ax_array);
        ay_old = detrend(var.body.ay_array);
        az_old = detrend(var.body.az_array);

        vx_old = var.body.vx;
        vy_old = var.body.vy;
        vz_old = var.body.vz;

        update_mpu(mpu);

        ax_new = detrend(var.body.ax_array);
        ay_new = detrend(var.body.ay_array);
        az_new = detrend(var.body.az_array);

        var.body.vx += (ax_new + ax_old) / 2.0 * (float) var.delay_acc / 1000.0;
        var.body.vy += (ay_new + ay_old) / 2.0 * (float) var.delay_acc / 1000.0;
        var.body.vz += (az_new + az_old) / 2.0 * (float) var.delay_acc / 1000.0;

        var.body.x += (var.body.vx + vx_old) / 2.0 * (float) var.delay_acc / 1000.0;
        var.body.y += (var.body.vy + vy_old) / 2.0 * (float) var.delay_acc / 1000.0;
        var.body.z += (var.body.vz + vz_old) / 2.0 * (float) var.delay_acc / 1000.0;
    }

void BodyMovement::config_variables() {
	// ----- Variáveis globais -----
	var.t_samples = 0;
	var.stopped = true;
	var.dist = 0;
	var.i = 0;
	var.aux_ac = 0;
	var.average = 0;
    var.body.max_z = -1e5;
    var.body.min_z = 1e5;
	var.arrow = ARROW_UP; // this is the y position of selection sign ">"
	var.page = FIRST_PAGE;	// page as default
	var.timerMillisButton = 0;
	var.timerBackButton = 0;
	var.lastState = HIGH; // the previous state from the input pin
	var.enterTestPage = false;
}

void BodyMovement::calibrate_imu(MPU6050 *mpu) {
    float x = 0, y = 0, z = 0, yaw = 0, pitch = 0, roll = 0;
    float x_offset, y_offset, z_offset, yaw_offset, pitch_offset, roll_offset;

    do {
        x_offset = mpu->getAccXoffset();
        y_offset = mpu->getAccYoffset();
        z_offset = mpu->getAccZoffset();
        yaw_offset = mpu->getGyroZoffset();
        pitch_offset = mpu->getGyroXoffset();
        roll_offset = mpu->getGyroYoffset();

        x = 0; 
        y = 0; 
        z = 0; 
        yaw = 0; 
        pitch = 0; 
        roll = 0;

        for (int i = 0; i < CALIBRATION_SIZE; i++) {
            mpu->update();

            z += mpu->getAccZ()/CALIBRATION_SIZE;
            y += mpu->getAccY()/CALIBRATION_SIZE;
            x += mpu->getAccX()/CALIBRATION_SIZE;

            yaw += mpu->getGyroZ()/CALIBRATION_SIZE;
            roll += mpu->getGyroY()/CALIBRATION_SIZE;
            pitch += mpu->getGyroX()/CALIBRATION_SIZE;

            delay(1);
        }

        z -= 1;

        // acc = reading - offset_old - offset_new
        mpu->setAccOffsets(x_offset + x/4, y_offset + y/4, z_offset + z/4);
        mpu->setGyroOffsets(pitch_offset + pitch/4, roll_offset + roll/4, yaw_offset + yaw/4);

    } while (abs(x) > 1e-2 || abs(y) > 1e-2 || abs(z) > 1e-2 || abs(yaw) > 1e-2 || abs(pitch) > 1e-2 || abs(roll) > 1e-2);

    do {
        x = millis();

        mpu->update();
        var.mpu.mean_acc_x = mpu->getAccX();
        var.mpu.mean_acc_y = mpu->getAccY();
        var.mpu.mean_acc_z = mpu->getAccZ();
        var.mpu.mean_roll = mpu->getAngleY();
        var.mpu.mean_pitch = mpu->getAngleX();
        var.mpu.mean_yaw = mpu->getAngleZ();

        x = millis() - x;
        delay(var.delay_acc > x ? var.delay_acc - x : 0);
        std::cout << abs(var.body.az) << std::endl;
    } while (abs(var.body.az) > 1e-3 || abs(var.body.az - var.mpu.z[FILTER_ORDER - 2]) > 1e-2);
}

float BodyMovement::apply_filter(float *x, float *y) {
    // Filtering
    float Y = 0;
    for (int i = 0; i < FILTER_ORDER; i++) {
        Y += B[i] * x[6 - i];
        if (i > 0) {
            Y -= A[i] * y[6 - i];
        }
    }
    Y = Y/A[0];

    return Y;
}

float BodyMovement::detrend(float *x) {
    // Detrending
    float a, b;
    float sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0;

    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum_y += x[i];
        sum_x += i;
        sum_xx += i * i;
        sum_xy += i * x[i];
    }
    
    sum_xy -= sum_x*sum_y/ARRAY_SIZE;
    sum_xx -= sum_x*sum_x/ARRAY_SIZE;

    a = sum_xy/sum_xx;
    b = sum_y/ARRAY_SIZE - a*sum_x/ARRAY_SIZE;

    return x[ARRAY_SIZE - 1] - a*(ARRAY_SIZE - 1) - b;
}