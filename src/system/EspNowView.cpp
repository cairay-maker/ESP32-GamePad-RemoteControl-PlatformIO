#include "system/EspNowView.h"
#include "hal/ESPNowHandler.h" // Needed to access the global espNow object
#include <WiFi.h>

// Link to the global object defined in main.cpp
extern ESPNowHandler espNow;

EspNowView::EspNowView(TFTHandler& tftRef, Hardware& hwRef) 
    : Activity(tftRef, hwRef) {}

void EspNowView::init() {}

void EspNowView::enter() {
    // 1. Show a message so the user knows to hold still
    tft.canvas.fillSprite(TFT_BLACK);
    tft.drawCenteredText("CALIBRATING IMU", 50, TFT_YELLOW, 1);
    tft.drawCenteredText("HOLD STILL...", 70, TFT_WHITE, 1);
    tft.updateDisplay();

    // 2. Call the calibration function (this takes ~300ms)
    hw.imu.calibrate();
    
    lastDisplayedCount = espNow.sentCount;
    draw(); 
}

void EspNowView::draw() {
    PacketData& p = espNow.lastSentPacket;

    tft.canvas.fillSprite(TFT_BLACK);

    tft.drawCenteredText("ESP-NOW BROADCAST", 8, TFT_CYAN, 1);
    
    // Header Info
    tft.canvas.setTextColor(TFT_WHITE);
    tft.canvas.setCursor(5, 20);
    tft.canvas.printf("Mode: Broadcast  CH: %d", WiFi.channel() == 0 ? 1 : WiFi.channel());
    tft.canvas.setCursor(5, 30);
    tft.canvas.println("Addr: FF:FF:FF:FF:FF:FF");
    tft.canvas.setCursor(5, 40);
    tft.canvas.println("SW2 UP to Transmit"); 

    //tft.canvas.drawFastHLine(5, 54, 150, TFT_DARKGREY);

    tft.drawCenteredText("LAST SENT PACKET", 55, TFT_CYAN, 1);
    
    // 1. Joysticks (Displayed as floats from the packet)
    tft.canvas.setTextColor(TFT_ORANGE);
    tft.canvas.setCursor(5, 65);
    tft.canvas.printf("RX:%+4.1f Y:%+4.1f EL:%+2d R:%+2d", p.rx, p.ry, p.encL, p.encR);
    // 2. IMU Data (Interpreted from packet)
    tft.canvas.setTextColor(TFT_GREEN);
    tft.canvas.setCursor(5, 75);
    tft.canvas.printf("Pot-L:%+4.1f M:%+4.1f R:%+4.1f", p.pL, p.pM, p.pR);
    tft.canvas.setTextColor(TFT_MAGENTA);
    tft.canvas.setCursor(5, 85);
    tft.canvas.printf("IMU-P:%+4.1f R:%+4.1f Y:%+4.1f", p.pitch, p.roll, p.yaw);

    // 3. Buttons (Interpret Text + 8-bit visual pattern)
    tft.canvas.setTextColor(TFT_YELLOW);
    tft.canvas.setCursor(5, 95);
    if (p.buttons == 0) {
            tft.canvas.print("NONE");
        } else {
            // Check individual bits and print the name
            // This handles multiple buttons by printing them side-by-side
            if (p.buttons & (1 << 0)) tft.canvas.print("UP ");
            if (p.buttons & (1 << 1)) tft.canvas.print("DOWN ");
            if (p.buttons & (1 << 2)) tft.canvas.print("LEFT ");
            if (p.buttons & (1 << 3)) tft.canvas.print("RIGHT ");
            if (p.buttons & (1 << 4)) tft.canvas.print("SELECT ");
            
            // You can even show the toggle switches
            if (p.buttons & (1 << 7)) tft.canvas.print("SW1 ");
            if (p.buttons & (1 << 8)) tft.canvas.print("SW2 ");
        }
    
    tft.canvas.setCursor(5, 105);
    tft.canvas.print("8 Bits: ");
    for (int i = 8; i >= 0; i--) {
        tft.canvas.print((p.buttons & (1 << i)) ? "1" : "0");
    }

    tft.drawCenteredText("HOLD SELECT TO EXIT", 118, TFT_DARKGREY, 1);
    tft.updateDisplay();
}



void EspNowView::update() {
    static unsigned long lastRefresh = 0;
    
    // 1. Only check every 100ms (10 times per second) for responsiveness
    if (millis() - lastRefresh >= 100) {
        
        // 2. Check if a new packet has actually gone out
        if (espNow.sentCount != lastDisplayedCount) {
            
            // Sync the tracker so we don't draw again until the NEXT packet
            lastDisplayedCount = espNow.sentCount;
            
            // Trigger the full redraw with the new data from p
            draw(); 
            
            lastRefresh = millis();
        }
    }
}

void EspNowView::exit() {}