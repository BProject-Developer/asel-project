#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// pin
#define SOIL_PIN 27
#define RELAY_PIN 14
#define OLED_SDA 21
#define OLED_SCL 22

// setting relay
#define RELAY_ACTIVE_LOW false

// oled
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// wifi + blynk
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// variabel
bool autoMode = true;   // default auto
bool relayState = false;

// kalibrasi
const int SOIL_DRY_RAW = 3000;  
const int SOIL_WET_RAW = 1200;  
const int THRESHOLD = 50; 

BlynkTimer timer;

// set relay
void setRelay(bool state) {
  relayState = state;
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, state ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  }
}

// read sensor
int getSoilMoisture() {
  int raw = analogRead(SOIL_PIN);
  int percent = map(raw, SOIL_DRY_RAW, SOIL_WET_RAW, 0, 100);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  return percent;
}

// oled
void updateOLED(int soilPercent) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("Soil: ");
  display.print(soilPercent);
  display.println("%");

  display.setCursor(0,16);
  display.print("Mode: ");
  display.println(autoMode ? "AUTO" : "MANUAL");

  display.setCursor(0,32);
  display.print("Pump: ");
  display.println(relayState ? "ON" : "OFF");

  display.display();
}

// send data2blynk
void sendToBlynk() {
  int soilPercent = getSoilMoisture();

  if (autoMode) {
    if (soilPercent < THRESHOLD) {
      setRelay(true);
    } else {
      setRelay(false);
    }
  }
  updateOLED(soilPercent);
  Blynk.virtualWrite(V2, soilPercent); 
}

// blynk set
BLYNK_WRITE(V0) {  
  autoMode = param.asInt(); // 1 = auto, 0 = manual
}

BLYNK_WRITE(V1) {  
  if (!autoMode) {
    int val = param.asInt();
    setRelay(val);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED gagal!"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}
