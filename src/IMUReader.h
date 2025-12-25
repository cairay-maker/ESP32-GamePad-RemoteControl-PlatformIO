#ifndef IMUREADER_H
#define IMUREADER_H

#include <Adafruit_LSM6DS33.h>

class IMUReader {
public:
  bool begin();
  void read();
  float ax, ay, az, gx, gy, gz;

private:
  Adafruit_LSM6DS33 lsm6ds;
};

#endif