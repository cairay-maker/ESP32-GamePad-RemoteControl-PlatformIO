#include "hal/ESPNowHandler.h"
#include "hal/Hardware.h" // Needed to access hw.state and hw.imu

// Track initialization state globally within this file
static bool isEspNowStarted = false;

ESPNowHandler::ESPNowHandler() {
    // Initialize buffers with zeros
    memset(&packet, 0, sizeof(PacketData));
    memset(&lastSentPacket, 0, sizeof(PacketData));
}

void ESPNowHandler::begin() {
    if (isEspNowStarted) return;

    // Standard ESP-NOW setup for ESP32
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // Stay on a fixed channel, don't hunt for APs

    if (esp_now_init() != ESP_OK) {
        Serial.println("!!! ESP-NOW: Initialization Failed");
        return;
    }

    // Register callback to monitor send status
    esp_now_register_send_cb(onDataSent);

    // Register Peer (Broadcast Address)
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0; // Use current WiFi channel
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("!!! ESP-NOW: Failed to add peer");
        return;
    }

    isEspNowStarted = true;
    Serial.println(">>> ESP-NOW: Ready (Broadcast Mode)");
}

void ESPNowHandler::send(Hardware& hw) {
    // Safety: Don't try to send if Radio is OFF (Switch 2 down)
    if (WiFi.getMode() == WIFI_OFF) return;

    // 1. Map Hardware State to Packet
    packet.lx = hw.state.joyLX;
    packet.ly = hw.state.joyLY;
    packet.rx = hw.state.joyRX;
    packet.ry = hw.state.joyRY;
    
    packet.pL = hw.state.potL;
    packet.pM = hw.state.potM;
    packet.pR = hw.state.potR;

    packet.encL = (int16_t)hw.state.encL;
    packet.encR = (int16_t)hw.state.encR;
    packet.buttons = hw.state.buttons;

    // 2. Map Live IMU data
    packet.pitch = hw.imu.getPitch();
    packet.roll  = hw.imu.getRoll();
    packet.yaw   = hw.imu.getYawRate();

    // 3. Transmit
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &packet, sizeof(packet));

    if (result == ESP_OK) {
        // Copy to "Last Sent" buffer so the UI can display exactly what went out
        memcpy(&lastSentPacket, &packet, sizeof(PacketData));
        sentCount++;
    }
}

// Callback function to see if the packet actually left the radio
void ESPNowHandler::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Optional: Log status for debugging
    // Serial.print("\rLast Packet Delivery Status: ");
    // Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}