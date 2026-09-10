# Smart Dustbin (IoT)

An IoT-based smart trash bin built on the ESP32 that opens its lid automatically, tracks how full it is in real time, and sends a notification before it overflows — no physical contact needed.

🎥 [Demo video](https://youtube.com/shorts/Y0V8Ud9-Y8s?feature=share)

## What it does

- **Hands-free lid**: an IR sensor detects a hand or object nearby and triggers a servo motor to open the lid, which closes automatically after 3 seconds.
- **Fill-level tracking**: an ultrasonic sensor (HC-SR04) measures the distance to the trash inside and converts it to a 0–100% fill level.
- **Remote monitoring**: distance, fill level, and lid angle are streamed live to the [Blynk](https://blynk.io) IoT dashboard over Wi-Fi.
- **Overflow alerts**: once the bin hits 85% full, Blynk sends a push/email notification so it gets emptied before it overflows. The alert only fires once per fill cycle.

## Hardware

| Component | Purpose |
|---|---|
| ESP32 | Main microcontroller, Wi-Fi + cloud connection |
| HC-SR04 ultrasonic sensor | Measures fill level |
| IR sensor | Detects hand/object to trigger the lid |
| Servo motor | Opens/closes the lid |

## Software / Platform

- Arduino IDE (C++)
- Blynk IoT platform (dashboard, datastreams, event notifications)
- Libraries: `WiFi.h`, `WiFiClient.h`, `BlynkSimpleEsp32.h`, `ESP32Servo.h`

## How it works

1. On boot, the ESP32 connects to Wi-Fi and the Blynk cloud, and initializes the sensors and servo (lid starts closed at 0°).
2. Every loop, two timed routines run:
   - `ultrasonic()` reads the distance every 500ms, maps it to a 0–100% bin level, and pushes both values to Blynk (`V0` = distance, `V1` = bin level). If the level crosses 85%, it fires a one-time `bin_full_notification` event.
   - `SMESensor()` polls the IR sensor every 100ms. When triggered, it opens the lid (servo to 180°, `V2` updated), then closes it again after 3 seconds.
3. All of this is visible live on a Blynk dashboard — see the screenshots/report for the datastream and event setup.

## Setup

1. Clone this repo and open `Smart_Dustbin.ino` in Arduino IDE.
2. Copy `secrets.h.example` to a new file named `secrets.h` in the same folder, and fill in your own values:
   ```cpp
   #define BLYNK_AUTH_TOKEN "your_blynk_auth_token_here"
   #define WIFI_SSID "your_wifi_name"
   #define WIFI_PASS "your_wifi_password"
   ```
   `secrets.h` is gitignored and never committed — this keeps credentials out of the public repo.
3. In your Blynk console, create a template with three integer datastreams: `V0` (distance), `V1` (bin level), `V2` (servo angle), and an event named `bin_full_notification` for the overflow alert.
4. Wire up the hardware (ultrasonic on pins 33/32, servo on pin 26, IR sensor on pin 14 — adjust in code if your wiring differs), then upload the sketch to your ESP32.

## Challenges & what I'd improve

- **Sensor accuracy**: reflective or irregularly shaped trash occasionally threw off the ultrasonic readings, which affected the calculated fill %.
- **Servo power**: the servo sometimes couldn't reach a full 180° because the ESP32's VIN output couldn't always supply enough current, causing partial opens/jitter. A dedicated 5V supply would fix this.
- **Wi-Fi stability**: on a weak connection, sensor data to Blynk would occasionally lag or drop momentarily.
- **Next steps I'd explore**: a dedicated power supply for the servo, logging data to something like Firebase/ThingSpeak for longer-term trends, and adding a weight or gas sensor for richer monitoring.

## Team

This was a group project for *ITE233: Introduction to IoTs* at Stamford International University. I implemented the ultrasonic sensing/fill-level logic and Blynk notification handling; lid/servo control and dashboard setup were shared with teammates.

## References

- [Blynk documentation](https://docs.blynk.io)
- [ESP32 + HC-SR04 ultrasonic tutorial — Random Nerd Tutorials](https://randomnerdtutorials.com/esp32-hc-sr04-ultrasonic-arduino/)
- [Arduino basic servo control](https://docs.arduino.cc/tutorials/generic/basic-servo-control/)
