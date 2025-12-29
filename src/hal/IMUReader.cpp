#include "IMUReader.h"
#include <Wire.h>
#include <Arduino.h>

bool IMUReader::begin() {
    Wire.begin(21, 22);
    Wire.setClock(400000); 
    if (!lsm6ds.begin_I2C(0x6B)) return false;

    lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
    lsm6ds.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
    lsm6ds.setAccelDataRate(LSM6DS_RATE_208_HZ);
    lsm6ds.setGyroDataRate(LSM6DS_RATE_208_HZ);

    lastMicros = micros();
    return true;
}

void IMUReader::calibrate() {
    float pSum = 0, rSum = 0, gxb = 0, gyb = 0, gzb = 0;
    const int samples = 100;

    for (int i = 0; i < samples; i++) {
        sensors_event_t a, g, t;
        lsm6ds.getEvent(&a, &g, &t);
        
        pSum += atan2(-a.acceleration.y, -a.acceleration.z) * 57.2958f;
        rSum += atan2(-a.acceleration.x, -a.acceleration.z) * 57.2958f;
        gxb += g.gyro.x;
        gyb += g.gyro.y;
        gzb += g.gyro.z;
        delay(2); 
    }

    pitchOffset = pSum / samples;
    rollOffset = rSum / samples;
    gxBias = gxb / samples;
    gyBias = gyb / samples;
    gzBias = gzb / samples;

    // Reset filter states so they don't "drift" to the new zero
    pitch = 0; 
    roll = 0;
    filteredYawRate = 0;
}

void IMUReader::update() {
    sensors_event_t accel, gyro, temp;
    lsm6ds.getEvent(&accel, &gyro, &temp);

    ax = accel.acceleration.x; ay = accel.acceleration.y; az = accel.acceleration.z;
    
    // 1. Apply Gyro Bias Correction (The Calibration)
    gx = gyro.gyro.x - gxBias;
    gy = gyro.gyro.y - gyBias;
    gz = gyro.gyro.z - gzBias;

    unsigned long currentMicros = micros();
    float dt = (currentMicros - lastMicros) / 1000000.0f;
    lastMicros = currentMicros;

    // 2. Converto to Degrees/sec
    float gx_deg = -gx * 57.2958f; 
    float gy_deg =  gy * 57.2958f;
    float gz_deg =  gz * 57.2958f;

    // 3. Accelerometer "Raw" Angles
    float accPitch = atan2(-ay, -az) * 57.2958f;
    float accRoll  = atan2(-ax, -az) * 57.2958f;

    // 4. Complementary Filter (Pitch and Roll)
    pitch = alpha * (pitch + gx_deg * dt) + (1.0f - alpha) * accPitch;
    roll  = alpha * (roll + gy_deg * dt) + (1.0f - alpha) * accRoll;

    // 5. Low Pass Filter (Yaw Rate Smoothing to stop flickering)
    filteredYawRate = (filteredYawRate * (1.0f - yawAlpha)) + (gz_deg * yawAlpha);
}