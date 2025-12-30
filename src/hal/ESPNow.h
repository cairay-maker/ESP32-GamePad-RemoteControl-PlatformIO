#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "Hardware.h"

// Fixed-size packet for consistent alignment across different ESP32s
typedef struct {
    int16_t lx, ly, rx, ry; 
    uint16_t pL, pM, pR;    
    uint32_t buttons;       
    int16_t encL, encR;     
} PacketData;

class ESPNowHandler {
public:
    void begin();
    void send(const ControllerState& state);
private:
    PacketData packet;
    static void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
};

#endif