Markdown

Started: Dec 22, 2025 | Pop!_OS Linux + PlatformIO in VS Code 

## Hardware
[ESP32-WROOM-32 30-pin USB-C board](https://europe1.discourse-cdn.com/arduino/original/4X/7/7/e/77ead2a2a7d13b189ad9b6ba24c1d55b2c3e4645.jpeg)  

## Pin Assignment
  Toggle Swith: GPIO-19; added external 5kΩ pull-up from GPIO19 to 3.3V. The pull-up is now ~10× stronger than the internal 45K one alone

  TFT_SCLK      GPIO-18
  TFT_MOSI      GPIO-23
  TFT_RST       GPIO-16
  TFT_DC        GPIO-17 
  TFT_CS        GPIO-5






## ⚙️ Hardware Setup
- **Board Pinout Notes** (30 pins total, ~25 usable GPIOs):
  | Label | GPIO | Notes |
  |-------|------|-------|
  | Onboard LED | 2 | Active-low blink works! |
  | TX | 1 | Avoid for LED (some boards flicker Serial) |
  | Safe GPIOs | 4,5,12-19,21-23,25-27,32-33 | For displays/SPI/I2C |
  | Input-only | 34-39 | No output/pullup |
  - Full ref: [Random Nerd Tutorials ESP32 Pinout](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/) <web:2>
- **USB/Serial**: /dev/ttyACM0 (native USB on some; fixed with udev rules + dialout group). Upload: Hold BOOT if "Connecting..." hangs.
- **Power**: USB-C 5V data cable (not charge-only!). Tested on Pop!_OS.

**Future Displays**:
- TFT (e.g., ILI9341/ST7789): ~6-8 SPI pins (CS=5, DC=4, RST=2?, MOSI=23, SCK=18, etc.).
- OLED (SSD1306 128x64 I2C): SDA=21, SCL=22.
- Wiring: Add table/photo when connected.

## 📁 Software Setup (PlatformIO)
Copy this `platformio.ini` (update port if needed):
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev  ; ESP32-WROOM-32 30-pin (DOIT DevKit V1 compatible)
framework = arduino
monitor_speed = 115200
upload_port = /dev/ttyACM0  ; or /dev/serial/by-id/usb-Espressif_... for multi-boards
monitor_port = /dev/ttyACM0
lib_deps = 
    bodmer/TFT_eSPI  ; For TFT displays (configure User_Setup.h)
    olikraus/U8g2    ; For OLED monochrome

; Build flags if needed (e.g., for TFT_eSPI)
build_flags = -DUSER_SETUP_LOADED=1