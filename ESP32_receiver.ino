#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// شاشة I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi
const char* ssid = "iPhone Amna";
const char* password = "------";

// HiveMQ
const char* mqtt_server = "83781419b6b8401e84ed76718a176eab.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

const char* mqtt_user = "ESP32user";
const char* mqtt_password = "------";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  int rssi = message.toInt();

  String statusText;

  if (rssi >= -60)
    statusText = "STRONG";
  else if (rssi >= -75)
    statusText = "MEDIUM";
  else
    statusText = "WEAK";

  Serial.print("RSSI: ");
  Serial.println(rssi);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("RSSI:");
  lcd.print(rssi);

  lcd.setCursor(0, 1);
  lcd.print(statusText);
}

void setup_wifi() {

  Serial.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    if (client.connect("ESP32_Receiver", mqtt_user, mqtt_password)) {

      Serial.println("Connected");

      client.subscribe("signal/rssi");

    } else {

      Serial.print("Failed rc=");
      Serial.println(client.state());

      delay(3000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Waiting RSSI");

  setup_wifi();

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}