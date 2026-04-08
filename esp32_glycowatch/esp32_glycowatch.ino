#include <WiFi.h>
#include <PubSubClient.h>

// WIFI
const char* ssid = "ESCOBAR-2G";
const char* password = "WIFI-PASSWORD";

// MQTT
const char* mqtt_server = "test.mosquitto.org";
const char* topic = "glycowatch/device/ESP32-ABC-123";

// DEVICE DATA
const char* deviceIdentifier = "ESP32-ABC-123";
const char* apiKey = "zUExsMdJ23LHBySUW69o37vie4ZbVmH3fzXZA-mJTKU"; // REEMPLAZAR

WiFiClient espClient;
PubSubClient client(espClient);

// contador para evitar duplicados
int counter = 0;

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

  int glucose = random(80, 200);

  counter++;

  String sourceEventId = "esp32-" + String(counter);

  String payload = "{";
  payload += "\"deviceIdentifier\":\"" + String(deviceIdentifier) + "\",";
  payload += "\"apiKey\":\"" + String(apiKey) + "\",";
  payload += "\"glucoseMgDl\":" + String(glucose) + ",";
  payload += "\"measuredAt\":\"2026-04-07T12:00:00Z\",";
  payload += "\"sourceEventId\":\"" + sourceEventId + "\",";
  payload += "\"origin\":\"esp32-mqtt\"";
  payload += "}";

  client.publish(topic, payload.c_str(), false);

  Serial.println("Enviado:");
  Serial.println("{");
  Serial.println("  \"deviceIdentifier\": \"" + String(deviceIdentifier) + "\",");
  Serial.println("  \"apiKey\": \"" + String(apiKey) + "\",");
  Serial.println("  \"glucoseMgDl\": " + String(glucose) + ",");
  Serial.println("  \"measuredAt\": \"2026-04-07T12:00:00Z\",");
  Serial.println("  \"sourceEventId\": \"" + sourceEventId + "\",");
  Serial.println("  \"origin\": \"esp32-mqtt\"");
  Serial.println("}");
  Serial.println("----------------------");

  delay(10000); // cada 10 segundos
}