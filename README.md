# ⏱️ Smart Desk Timer (ESP32)

An automated, touchless Pomodoro timer built with an ESP32 microcontroller, an IR obstacle sensor, an HC-SR501 PIR motion sensor, and a 16x2 I2C LCD. The system combines hand-gesture controls with presence-based motion detection to automatically track focus sessions and pauses without requiring manual button presses.

---

## 🌟 Key Features

* **Touchless Gesture Control (IR Sensor):**
  * **Short Wave (< 1.5s):** Start or toggle manual Pause/Resume.
  * **Long Hold (>= 3s):** Hard-reset system to `IDLE` state (`30:00`).
* **Automated Desk Presence Auto-Pause (PIR Sensor):**
  * Tracks physical presence at your desk.
  * Automatically pauses the countdown if no movement is detected for 30 seconds (`AWAY (AUTO-PAUSE)`).
  * Automatically resumes the countdown when you return to your seat.
* **Integrated 30/5 Pomodoro Cycle:**
  * **Focus Session:** 30 minutes countdown.
  * **Mandatory Break:** Automatically switches to a 5-minute break prompt (`LEAVE & TAKE REST`).
* **Non-Blocking Architecture:** Written in embedded C++ using a Finite State Machine (FSM) driven by `millis()` rather than blocking `delay()` statements.

---

## 🛠️ Hardware & Pinout Mapping

| Component | Component Pin | ESP32 Pin | Function / Protocol |
| :--- | :--- | :--- | :--- |
| **16x2 LCD (I2C)** | SDA | GPIO 21 | Hardware I2C Data |
| | SCL | GPIO 22 | Hardware I2C Clock |
| | VCC / GND | 5V / GND | Power |
| **IR Obstacle Sensor** | OUT | GPIO 33 | Digital Input (Active-LOW) |
| | VCC / GND | 5V (or 3.3V) / GND | Power |
| **PIR Sensor (HC-SR501)**| OUT | GPIO 27 | Digital Input (Active-HIGH) |
| | VCC / GND | 5V / GND | Power |

---

## ⚙️ Calibration & Physical Setup

For the software-level 30-second timeout to function properly, the hardware potentiometers on the back of the HC-SR501 PIR sensor must be adjusted:

1. **Time Delay Trimpot (Right Knob):** Turn **fully counter-clockwise**. This reduces the hardware signal hold duration to its minimum (~2–3 seconds), handing timing control over to the ESP32 code.
2. **Sensitivity Trimpot (Left Knob):** Adjust to roughly **50% (middle position)** to limit the detection field strictly to your desk area and avoid false triggers from background room movement.
3. **Trigger Jumper:** Ensure the yellow jumper cap is set to **H** (Repeat Trigger mode).

---

## 🔄 State Machine Logic

The firmware operates across six distinct operational states:
