#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ESCOBAR-2G";
const char* password = "PASSWORD-WIFI";

const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Conectando a WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");

    if (client.connect("ESP32Client123")) {
      Serial.println("conectado!");
    } else {
      Serial.print("fallo, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int glucose = random(80, 200); // simulación

  String payload = "{";
  payload += "\"deviceId\": \"10\",";
  payload += "\"glucoseMgDl\": " + String(glucose) + ",";
  payload += "\"measuredAt\": \"2026-04-07T12:00:00Z\"";
  payload += "}";

  client.publish("glycowatch/device/10", payload.c_str());

  Serial.println("Enviado:");
  Serial.println("{");
  Serial.println("  \"deviceId\": \"10\",");
  Serial.println("  \"glucoseMgDl\": " + String(glucose) + ",");
  Serial.println("  \"measuredAt\": \"2026-04-07T12:00:00Z\"");
  Serial.println("}");
  Serial.println("----------------------");

  delay(5000);
}