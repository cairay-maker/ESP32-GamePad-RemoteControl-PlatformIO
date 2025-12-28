#ifndef IMUREADER_H
#define IMUREADER_H

#include <Adafruit_LSM6DS33.h>

class IMUReader {
public:
    bool begin();
    void update();
    void calibrate();

    // Raw Data
    float ax, ay, az, gx, gy, gz;

    // Filtered Output (Calibrated)
    float getPitch() const { return pitch - pitchOffset; }
    float getRoll() const  { return roll - rollOffset; }
    float getYawRate() const { return filteredYawRate; } // Returns the smooth, zeroed value

    // Raw comparison helpers
    float getRawPitch() const { return (atan2(-ay, -az) * 57.2958f) - pitchOffset; }
    float getRawRoll() const  { return (atan2(-ax, -az) * 57.2958f) - rollOffset; }

private:
    Adafruit_LSM6DS33 lsm6ds;
    float pitch = 0, roll = 0;
    float filteredYawRate = 0; // Smoothing variable for Yaw
    unsigned long lastMicros;
    
    const float alpha = 0.96f;      // Filter for Pitch/Roll
    const float yawAlpha = 0.15f;   // Smoothing for Yaw Rate (Low Pass)

    // Calibration Offsets
    float pitchOffset = 0;
    float rollOffset = 0;
    float gxBias = 0, gyBias = 0, gzBias = 0;
};

#endif