#ifndef ESPNOW_HANDLER_H
#define ESPNOW_HANDLER_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Forward declaration so we don't need to include the full Hardware.h here
class Hardware;

// The exact data structure sent over the air
typedef struct {
    float lx, ly, rx, ry;    // Joysticks (-1.0 to 1.0)
    float pL, pM, pR;       // Potentiometers
    float roll, pitch, yaw;  // IMU Data
    int16_t encL, encR;      // Encoders
    uint32_t buttons;        // Button bitmask
} PacketData;

class ESPNowHandler {
public:
    ESPNowHandler();
    
    void begin();
    void send(Hardware& hw);
    
    // Monitoring variables for the UI
    uint32_t sentCount = 0;
    PacketData lastSentPacket; 

private:
    PacketData packet; // Working buffer
    uint8_t receiverAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast
    
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
};

#endif