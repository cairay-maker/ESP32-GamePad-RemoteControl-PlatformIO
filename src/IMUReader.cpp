#include "IMUReader.h"
#include <Wire.h>

bool IMUReader::begin() {
  Wire.begin(21, 22);  // SDA=21, SCL=22
  delay(100);  // Small delay after begin
  Wire.setClock(400000);  // ← ADD THIS LINE (400kHz Fast Mode)
  if (!lsm6ds.begin_I2C(0x6B)) {
    return false;
  }
  lsm6ds.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
  lsm6ds.setGyroRange(LSM6DS_GYRO_RANGE_1000_DPS);
  lsm6ds.setAccelDataRate(LSM6DS_RATE_208_HZ);
  lsm6ds.setGyroDataRate(LSM6DS_RATE_208_HZ);
  return true;
}

void IMUReader::read() {
  sensors_event_t accel, gyro, temp;
  lsm6ds.getEvent(&accel, &gyro, &temp);
  ax = accel.acceleration.x;
  ay = accel.acceleration.y;
  az = accel.acceleration.z;
  gx = gyro.gyro.x;
  gy = gyro.gyro.y;
  gz = gyro.gyro.z;
}