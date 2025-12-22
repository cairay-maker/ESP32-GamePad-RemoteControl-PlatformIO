#include <Arduino.h>
#include <TFT_eSPI.h>

#define LED_PIN    2
#define SWITCH_PIN 19

TFT_eSPI tft = TFT_eSPI();

float cubeVertices[8][3] = {
  {-30, -30, -30}, {30, -30, -30}, {30, 30, -30}, {-30, 30, -30},
  {-30, -30,  30}, {30, -30,  30}, {30, 30,  30}, {-30, 30,  30}
};

bool switchOn = false;
unsigned long lastRotate = 0;
float angle = 0;

// For status printing
unsigned long lastStatusPrint = 0;
const unsigned long statusInterval = 500;  // ms

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== ESP32 TFT Toggle Demo (with Status Print) ===");
  Serial.println("Switch OFF → Static Ball | Switch ON → Rotating Cube");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 60);
  tft.println("Flip Switch");
}

void drawStaticBall() {
  tft.fillScreen(TFT_BLACK);
  tft.fillCircle(80, 64, 40, TFT_RED);
  tft.fillCircle(80, 64, 30, TFT_YELLOW);
  tft.fillCircle(80, 64, 20, TFT_WHITE);

  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(20, 110);
  tft.println("BALL MODE");
}

void drawCubeFrame() {
  if (millis() - lastRotate < 50) return;

  lastRotate = millis();
  angle += 3;

  tft.fillScreen(TFT_BLACK);

  int projected[8][2];
  float zScale = 100;
  for (int i = 0; i < 8; i++) {
    float x = cubeVertices[i][0];
    float y = cubeVertices[i][1];
    float z = cubeVertices[i][2];

    float rx = x * cos(radians(angle)) - z * sin(radians(angle));
    float rz = x * sin(radians(angle)) + z * cos(radians(angle));
    float ry = y * cos(radians(angle * 0.5)) - rz * sin(radians(angle * 0.5));
    rz = y * sin(radians(angle * 0.5)) + rz * cos(radians(angle * 0.5));

    float perspective = zScale / (rz + 150);
    projected[i][0] = (int)(rx * perspective) + 80;
    projected[i][1] = (int)(ry * perspective) + 64;
  }

  uint16_t colors[12] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW,
                        TFT_CYAN, TFT_MAGENTA, TFT_WHITE, TFT_ORANGE,
                        TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW};
  int edges[12][2] = {{0,1},{1,2},{2,3},{3,0}, {4,5},{5,6},{6,7},{7,4},
                      {0,4},{1,5},{2,6},{3,7}};

  for (int i = 0; i < 12; i++) {
    int a = edges[i][0];
    int b = edges[i][1];
    tft.drawLine(projected[a][0], projected[a][1],
                 projected[b][0], projected[b][1], colors[i]);
  }

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 110);
  tft.println("CUBE MODE");
}

void loop() {
  bool currentRaw = digitalRead(SWITCH_PIN);
  bool currentSwitchOn = (currentRaw == LOW);

  if (currentSwitchOn != switchOn) {
    delay(50);
    if (digitalRead(SWITCH_PIN) == currentRaw) {
      switchOn = currentSwitchOn;

      if (switchOn) {
        Serial.println(">>> SWITCH CHANGED: ON  →  Entering CUBE mode");
        digitalWrite(LED_PIN, LOW);
      } else {
        Serial.println(">>> SWITCH CHANGED: OFF →  Entering BALL mode");
        digitalWrite(LED_PIN, HIGH);
        drawStaticBall();
      }
    }
  }

  if (switchOn) {
    drawCubeFrame();
  }

  // Periodic status update
  if (millis() - lastStatusPrint >= statusInterval) {
    lastStatusPrint = millis();

    Serial.print("Switch pin state: ");
    Serial.print(currentRaw == LOW ? "LOW (closed)" : "HIGH (open)");
    Serial.print(" | Interpreted as: ");
    Serial.print(switchOn ? "ON " : "OFF");
    Serial.print(" | Current mode: ");
    Serial.println(switchOn ? "ROTATING CUBE" : "STATIC BALL");
  }
}