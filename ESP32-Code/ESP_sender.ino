#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "iPhone Amna";
const char* password = "-----";

// HiveMQ Cloud
const char* mqtt_server = "83781419b6b8401e84ed76718a176eab.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

const char* mqtt_user = "ESP32user";
const char* mqtt_password = "------";

// TLS Client
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {

  Serial.println();
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT... ");

    if (client.connect("ESP32_Sender", mqtt_user, mqtt_password)) {

      Serial.println("Connected!");

    } else {

      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println();

      delay(3000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  setup_wifi();

  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  long rssi = WiFi.RSSI();

  String message = String(rssi);

  client.publish("signal/rssi", message.c_str());

  Serial.print("RSSI Sent: ");
  Serial.println(message);

  delay(3000);
}