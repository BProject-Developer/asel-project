#define BLYNK_TEMPLATE_ID "TMPL6hq62dcIg"
#define BLYNK_TEMPLATE_NAME "HydroFlow"
#define BLYNK_AUTH_TOKEN "Gz8gmyXlVIxEnxxJ-rXvK3TQDwKeiIFI"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin Setup
#define SOIL_PIN 27
#define RELAY_PIN 14
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Blynk WiFi
char ssid[] = "pinkyboy";
char pass[] = "12345678";

// Relay logic
#define RELAY_ACTIVE_LOW true   

// Variabel
int soilMoisture = 0;
int autoMode = 1;   // default Auto mode
int relayState = 0; // 0 = off, 1 = on

// Fungsi kontrol relay
void relayOn() {
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? LOW : HIGH);
  relayState = 1;
}

void relayOff() {
  digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
  relayState = 0;
}

// Blynk widget handler
BLYNK_WRITE(V0) { // Switch Auto/Manual
  autoMode = param.asInt();
}

BLYNK_WRITE(V1) { // Button Manual Pump
  int btn = param.asInt();
  if (autoMode == 0) { // hanya manual mode
    if (btn == 1) relayOn();
    else relayOff();
  }
}

void setup() {
  Serial.begin(115200);

  // Relay pin setup
  pinMode(RELAY_PIN, OUTPUT);
  delay(100);
  relayOff();   // pastikan relay mati dulu

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  // Baca soil moisture (0-100%)
  soilMoisture = map(analogRead(SOIL_PIN), 0, 4095, 100, 0);

  // Mode Auto
  if (autoMode == 1) {
    if (soilMoisture < 40) relayOn();
    else relayOff();
  }

  // Update Blynk
  Blynk.virtualWrite(V2, soilMoisture);
  Blynk.virtualWrite(V3, relayState);

  // Update OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Soil: ");
  display.print(soilMoisture);
  display.println("%");

  display.setCursor(0, 20);
  display.print("Mode: ");
  display.println(autoMode ? "Auto" : "Manual");

  display.setCursor(0, 40);
  display.print("Pump: ");
  display.println(relayState ? "ON" : "OFF");

  display.display();

  delay(500);
}
