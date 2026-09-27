#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// =================================================
// Wi-Fi
// =================================================

#define WIFI_SSID "protosem"
#define WIFI_PASS "Proto#123"

// =================================================
// Adafruit IO
// =================================================

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "sachinn__s"
#define AIO_KEY         "YOUR_NEW_ADAFRUIT_IO_KEY"

// =================================================
// RELAY
// =================================================

#define RELAY_PIN 23

// Relay logic
#define RELAY_ON  HIGH
#define RELAY_OFF LOW

// =================================================
// MQTT
// =================================================

WiFiClient client;

Adafruit_MQTT_Client mqtt(
  &client,
  AIO_SERVER,
  AIO_SERVERPORT,
  AIO_USERNAME,
  AIO_KEY
);

// =================================================
// BULB FEED
// =================================================

Adafruit_MQTT_Subscribe bulbControl =
  Adafruit_MQTT_Subscribe(
    &mqtt,
    AIO_USERNAME "/feeds/bulb-control"
  );

// =================================================
// CONNECT TO WIFI
// =================================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// =================================================
// CONNECT TO ADAFRUIT IO
// =================================================

void connectMQTT() {

  while (!mqtt.connected()) {

    Serial.print("Connecting to Adafruit IO...");

    int8_t ret = mqtt.connect();

    if (ret == 0) {
      Serial.println("Connected to Adafruit IO!");
    }
    else {
      Serial.print("Failed, error = ");
      Serial.println(mqtt.connectErrorString(ret));

      mqtt.disconnect();
      delay(5000);
    }
  }
}

// =================================================
// PROCESS BULB COMMAND
// =================================================

void processBulbCommand(String command) {

  command.trim();
  command.toLowerCase();

  Serial.print("Received command: ");
  Serial.println(command);

  // -------------------------
  // BULB ON
  // -------------------------

  if (command == "1" ||
      command == "on" ||
      command == "bulb on" ||
      command == "bulbon") {

    digitalWrite(RELAY_PIN, RELAY_ON);

    Serial.println(">>> BULB ON");
  }

  // -------------------------
  // BULB OFF
  // -------------------------

  else if (command == "0" ||
           command == "off" ||
           command == "bulb off" ||
           command == "bulb of" ||
           command == "bulboff" ||
           command == "of") {

    digitalWrite(RELAY_PIN, RELAY_OFF);

    Serial.println(">>> BULB OFF");
  }

  // -------------------------
  // UNKNOWN
  // -------------------------

  else {

    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}

// =================================================
// SETUP
// =================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32 BULB CONTROL");
  Serial.println("================================");

  // Relay setup
  pinMode(RELAY_PIN, OUTPUT);

  // Start with bulb OFF
  digitalWrite(RELAY_PIN, RELAY_OFF);

  // WiFi
  connectWiFi();

  // Subscribe to feed
  mqtt.subscribe(&bulbControl);

  // MQTT
  connectMQTT();

  Serial.println("System ready!");
}

// =================================================
// LOOP
// =================================================

void loop() {

  // Reconnect WiFi if necessary
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Reconnect MQTT if necessary
  if (!mqtt.connected()) {
    connectMQTT();
  }

  // Keep MQTT connection alive
  mqtt.processPackets(1000);

  // Check for new feed value
  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(100)) != NULL) {

    if (subscription == &bulbControl) {

      String command = String((char *)bulbControl.lastread);

      processBulbCommand(command);
    }
  }
}
