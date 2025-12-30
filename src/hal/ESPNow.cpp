#include "ESPNow.h"

// Broadcast address (0xFF...) sends to all listening devices on the same channel
// Replace with a specific MAC later if you want a private link
uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

void ESPNowHandler::begin() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Ensure we aren't trying to connect to a router

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW: Init Failed");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    // Register the receiver as a peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;  // Use current Wi-Fi channel
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("ESP-NOW: Failed to add peer");
    } else {
        Serial.println("ESP-NOW: Ready");
    }
}

void ESPNowHandler::send(const ControllerState& state) {
    // Pack the hardware state into the transmission packet
    packet.lx = (int16_t)state.joyLX;
    packet.ly = (int16_t)state.joyLY;
    packet.rx = (int16_t)state.joyRX;
    packet.ry = (int16_t)state.joyRY;
    packet.pL = (uint16_t)state.potL;
    packet.pM = (uint16_t)state.potM;
    packet.pR = (uint16_t)state.potR;
    packet.buttons = state.buttons;
    packet.encL = (int16_t)state.encL;
    packet.encR = (int16_t)state.encR;

    esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));
}

void ESPNowHandler::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Keep this empty to avoid Serial overhead during high-speed flight/driving
}