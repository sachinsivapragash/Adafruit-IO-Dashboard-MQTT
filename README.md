# ESP32 Adafruit IO Cloud Control

## ProtoSem — Week 7 | Task 2

This task demonstrates how to control an **ESP32-connected bulb remotely through the internet** using **Adafruit IO and MQTT**.

Unlike a local web server, the ESP32 does not need to be accessed through its local IP address. **Adafruit IO acts as the cloud platform and MQTT broker**, allowing a dashboard switch to send commands to the ESP32 from anywhere with an internet connection.

---

## Task Objective

The objective is to control the ESP32 **from anywhere**, not only from the local network.

The system uses:

**Adafruit IO Dashboard → MQTT Feed → ESP32 → Relay → Bulb**

When the dashboard switch is changed, the command is published to the `bulb-control` feed. The ESP32 subscribes to this feed and receives the command through the MQTT broker.

---

## Concepts Learned

### Adafruit IO

**Adafruit IO** is a cloud IoT platform that provides feeds and dashboards.

It allows users to create dashboard controls such as switches, gauges, and charts and connect them to IoT devices.

### MQTT

**MQTT (Message Queuing Telemetry Transport)** is a lightweight publish/subscribe communication protocol designed for IoT devices.

Instead of the dashboard communicating directly with the ESP32, both communicate through an MQTT broker.

### Publisher, Subscriber and Broker

The system contains three main roles:

* **Publisher:** Adafruit IO dashboard switch
* **Subscriber:** ESP32
* **Broker:** Adafruit IO

The dashboard publishes a command to the feed, and the ESP32 subscribes to receive that command.

### Topic / Feed

The MQTT communication takes place through a named channel.

In this project, the Adafruit IO feed is:

```text
bulb-control
```

The ESP32 subscribes to:

```text
username/feeds/bulb-control
```

---

## System Architecture

```text
┌─────────────────────────────┐
│     Adafruit IO Dashboard   │
│       Bulb Control Switch   │
└──────────────┬──────────────┘
               │
               │ Publish
               ▼
┌─────────────────────────────┐
│      Adafruit IO / MQTT     │
│           Broker            │
└──────────────┬──────────────┘
               │
               │ Subscribe
               ▼
┌─────────────────────────────┐
│            ESP32            │
│       MQTT Subscriber       │
└──────────────┬──────────────┘
               │
               │ GPIO 23
               ▼
┌─────────────────────────────┐
│       Relay Module          │
└──────────────┬──────────────┘
               │
               ▼
┌─────────────────────────────┐
│          AC Bulb            │
└─────────────────────────────┘
```

The dashboard publishes a value to the `bulb-control` feed. Adafruit IO routes the message through MQTT, and the ESP32 processes the received command to control the relay.

---

## Hardware Used

| Component                      | Purpose                        |
| ------------------------------ | ------------------------------ |
| ESP32 Dev Module (WROOM-32)    | Main IoT controller            |
| 5V Single-Channel Relay Module | Electrically switches the bulb |
| AC Bulb & Holder               | Output device                  |
| Jumper Wires                   | Circuit connections            |
| Mains Cable                    | AC power connection            |

The ESP32 controls the relay through **GPIO 23**.

---

## Software & Platforms

* **Arduino IDE 2.3.10**
* **Adafruit IO**
* **Adafruit IO Arduino Library**
* **Adafruit MQTT Library**
* **WiFi.h**
* **MQTT**
* Web browser for accessing the Adafruit IO dashboard

---

## Pin Configuration

| ESP32 Pin | Connected Component | Function             |
| --------- | ------------------- | -------------------- |
| GPIO 23   | Relay IN            | Relay control signal |
| 5V        | Relay VCC           | Relay power          |
| GND       | Relay GND           | Common ground        |

The relay switches the bulb based on the command received through MQTT.

---

## MQTT Communication

The ESP32 connects to the Adafruit IO MQTT server using:

```text
Server: io.adafruit.com
Port: 1883
```

The MQTT client is configured using the Adafruit IO username and authentication key.

The ESP32 subscribes to:

```text
username/feeds/bulb-control
```

When a new value is published to this feed, the ESP32 receives it and passes the command to the bulb-control processing function.

---

## Bulb Control Commands

The program accepts multiple command formats.

### Turn ON

```text
1
on
bulb on
bulbon
```

When an ON command is received:

```cpp
digitalWrite(RELAY_PIN, RELAY_ON);
```

The relay is activated and the bulb turns ON.

### Turn OFF

```text
0
off
bulb off
bulboff
of
```

When an OFF command is received:

```cpp
digitalWrite(RELAY_PIN, RELAY_OFF);
```

The relay is deactivated and the bulb turns OFF.

---

## Program Structure

The Arduino program is divided into several main sections.

### 1. Wi-Fi Connection

The `connectWiFi()` function connects the ESP32 to the configured Wi-Fi network.

```cpp
void connectWiFi()
```

The ESP32 waits until the Wi-Fi connection is established and then displays its IP address through the Serial Monitor.

### 2. Adafruit IO Connection

The `connectMQTT()` function establishes the MQTT connection with Adafruit IO.

```cpp
void connectMQTT()
```

If the connection fails, the ESP32 waits and attempts to reconnect.

### 3. Feed Subscription

The ESP32 subscribes to the `bulb-control` feed:

```cpp
mqtt.subscribe(&bulbControl);
```

This allows it to receive new dashboard commands.

### 4. Command Processing

The `processBulbCommand()` function receives the MQTT message and determines whether the bulb should be turned ON or OFF.

```cpp
void processBulbCommand(String command)
```

### 5. Relay Control

The relay is connected to:

```cpp
#define RELAY_PIN 23
```

The program uses:

```cpp
#define RELAY_ON  HIGH
#define RELAY_OFF LOW
```

to control the relay.

### 6. Reconnection

The main loop continuously checks both Wi-Fi and MQTT connections.

If either connection is lost, the ESP32 attempts to reconnect automatically.

---

## Working Process

```text
1. User opens Adafruit IO Dashboard
              ↓
2. User changes Bulb Control switch
              ↓
3. Dashboard publishes value
              ↓
4. Value reaches bulb-control MQTT feed
              ↓
5. Adafruit IO acts as MQTT broker
              ↓
6. ESP32 receives the subscribed message
              ↓
7. processBulbCommand() interprets the value
              ↓
8. GPIO 23 controls the relay
              ↓
9. Relay switches the AC bulb
```

---

## Configuration

Before uploading the program:

1. Create an **Adafruit IO account**.
2. Create a feed named:

```text
bulb-control
```

3. Create a dashboard.
4. Add a **Toggle** block.
5. Connect the Toggle block to the `bulb-control` feed.
6. Enter the Adafruit IO username in the program.
7. Enter the Adafruit IO authentication key.
8. Enter the Wi-Fi credentials.
9. Upload the program to the ESP32.
10. Open the Serial Monitor at **115200 baud**.
11. Verify that the ESP32 connects to Wi-Fi.
12. Verify that the ESP32 connects to Adafruit IO.
13. Use the dashboard switch to control the bulb.

The portfolio documentation specifies keeping the Adafruit IO key out of the public repository.

---

## Security Note

**Do not upload your real Adafruit IO key or Wi-Fi password to GitHub.**

Use placeholders in the public code, for example:

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

#define AIO_USERNAME "YOUR_ADAFRUIT_USERNAME"
#define AIO_KEY "YOUR_ADAFRUIT_IO_KEY"
```

This prevents private credentials from being exposed in the public repository.

---

## Testing

| Test                 | Expected Result                  |
| -------------------- | -------------------------------- |
| ESP32 powered ON     | ESP32 starts successfully        |
| Wi-Fi connection     | ESP32 connects to Wi-Fi          |
| MQTT connection      | ESP32 connects to Adafruit IO    |
| Dashboard switch ON  | Relay activates                  |
| Relay activated      | Bulb turns ON                    |
| Dashboard switch OFF | Relay deactivates                |
| Relay deactivated    | Bulb turns OFF                   |
| Wi-Fi interruption   | ESP32 attempts reconnection      |
| MQTT interruption    | ESP32 attempts MQTT reconnection |

---

## Local Network vs Cloud Control

### Task 1 — Local Control

```text
Browser
   ↓
Local Wi-Fi
   ↓
ESP32 Web Server
   ↓
LED
```

The browser and ESP32 need to communicate through the local network.

### Task 2 — Cloud Control

```text
Dashboard
   ↓
Adafruit IO
   ↓
MQTT
   ↓
ESP32
   ↓
Relay
   ↓
Bulb
```

The ESP32 connects outward to Adafruit IO, so the dashboard does not need to directly access the ESP32's local IP address.

This is the key difference between the local web-server approach and the cloud-based MQTT approach.

---

## Challenges & Fixes

### MQTT Authentication

An incorrect or outdated Adafruit IO key can prevent the ESP32 from connecting to the MQTT server.

**Solution:** Generate a new Adafruit IO key and update the `AIO_KEY` value.

### Feed Name

The feed name in the code must match the feed created in Adafruit IO.

The ESP32 subscribes using:

```text
username/feeds/bulb-control
```

### Dashboard Command Format

The dashboard may publish different values such as:

```text
ON / OFF
```

or:

```text
1 / 0
```

The program therefore accepts multiple command formats so that the relay can respond correctly.

---

## Learning Outcomes

Through this task, I learned:

* How to connect an ESP32 to Wi-Fi
* How cloud IoT platforms work
* How Adafruit IO dashboards operate
* How MQTT publish/subscribe communication works
* How an MQTT broker connects devices and applications
* How to subscribe an ESP32 to an MQTT feed
* How to control a relay using MQTT commands
* How to implement Wi-Fi and MQTT reconnection
* How cloud-based IoT control differs from local network control

---

## Final Result

The ESP32 successfully receives commands from an **Adafruit IO cloud dashboard through MQTT** and uses those commands to control a relay connected to an AC bulb.

The complete communication path is:

**Adafruit IO Dashboard → MQTT Broker → ESP32 → GPIO 23 → Relay → Bulb**

This demonstrates how an ESP32 can be controlled remotely through a **cloud-based IoT architecture**, without requiring direct access to the ESP32 through its local IP address.

---

## Project Information

**Program:** ProtoSem
**Week:** 7
**Task:** 2 of 5
**Project:** ESP32 Adafruit IO Cloud Control
**Microcontroller:** ESP32 Dev Module (WROOM-32)
**Cloud Platform:** Adafruit IO
**Communication Protocol:** MQTT
**MQTT Port:** 1883
**Relay Control Pin:** GPIO 23
**Output:** AC Bulb

---

## Repository

The complete project contains the ESP32 Arduino sketch and project documentation for implementing cloud-based bulb control using Adafruit IO and MQTT.
