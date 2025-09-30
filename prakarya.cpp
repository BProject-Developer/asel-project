
#include <Servo.h>

Servo pintu;

#define PIR_PIN  13   // pin sensor PIR
#define SERVO_PIN  14 // pin servo

bool gerakanTerdeteksi = false;
unsigned long waktuTerakhirGerakan = 0;
const unsigned long delayTutup = 5000; // 5 detik setelah tidak ada gerakan pintu menutup

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
}

void loop() {
  int statusPIR = digitalRead(PIR_PIN);

  if (statusPIR == HIGH && !gerakanTerdeteksi) {
    Serial.println("Gerakan terdeteksi -> Pintu membuka");
    pintu.attach(SERVO_PIN);
    pintu.write(90);  // buka pintu
    delay(1000);      // tunggu servo bergerak
    pintu.detach();   // lepas supaya tidak panas
    gerakanTerdeteksi = true;
    waktuTerakhirGerakan = millis();
  }

  // Jika sudah ada gerakan sebelumnya, cek waktu
  if (gerakanTerdeteksi && millis() - waktuTerakhirGerakan > delayTutup) {
    Serial.println("Tidak ada gerakan -> Pintu menutup");
    pintu.attach(SERVO_PIN);
    pintu.write(0);   // tutup pintu
    delay(1000);
    pintu.detach();
    gerakanTerdeteksi = false;
  }
}