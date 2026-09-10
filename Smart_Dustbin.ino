//Templates for blynk
#define BLYNK_TEMPLATE_ID "TMPL6TFIbpQoQ"
#define BLYNK_TEMPLATE_NAME "Smart Dustbin"

#define BLYNK_PRINT Serial

//Libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "secrets.h"

//Connect to WiFi
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASS;

#define trigPin 33
#define echoPin 32
#define servoPin 26
#define irPin 14

Servo servo;
BlynkTimer timer;

long duration;
int distance;
int binLevel;
bool lidOpen = false; 
unsigned long lidOpenTime = 0;
bool notificationSent = false;

// Ultrasonic sensor
void ultrasonic() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return;

  distance = duration * 0.034 / 2;
  binLevel = map(distance, 14, 0, 0, 100);
  binLevel = constrain(binLevel, 0, 100);

  Blynk.virtualWrite(V0, distance);
  Blynk.virtualWrite(V1, binLevel);

  // Notification (when bin is 90% full)
if (binLevel >= 85 && !notificationSent) {
    Blynk.logEvent("bin_full_notification", "Dustbin is going to be full. Take out the trash, please.");
    Serial.println("Notification sent: Bin is 85% full!");
    notificationSent = true;
  }

// Reset notification once bin is emptied below 85%
  if (binLevel < 85) {
    notificationSent = false;
  }
}


// IR sensor and servo
void SMESensor() {
  int ir = digitalRead(irPin);

  if (ir == LOW && !lidOpen) { // active LOW
    servo.write(180);
    Blynk.virtualWrite(V2, 180);
    lidOpen = true;
    lidOpenTime = millis();
    Serial.println("Lid opened");
  }

  // close after 3 seconds
  if (lidOpen && millis() - lidOpenTime >= 3000) {
    servo.write(0);
    Blynk.virtualWrite(V2, 0);
    lidOpen = false;
    Serial.println("Lid closed");
  }
}

// Setup
void setup() {
  Serial.begin(115200);

  servo.attach(servoPin);
  servo.write(0);

  pinMode(irPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Connecting to Wi-Fi and Blynk...");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(500L, ultrasonic);
  timer.setInterval(100L, SMESensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
