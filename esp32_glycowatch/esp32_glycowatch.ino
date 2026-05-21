#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/* =========================
   WIFI CONFIG
========================= */
const char* WIFI_SSID = "TU_WIFI";
const char* WIFI_PASSWORD = "TU_PASSWORD";

/* =========================
   HIVEMQ CLOUD CONFIG
========================= */
const char* MQTT_BROKER = "478aa6d5a50348a285de276f76d74233.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;

const char* MQTT_USERNAME = "glycowatch_backend";
const char* MQTT_PASSWORD = "TU_PASSWORD_MQTT";

/* =========================
   MQTT TOPIC
========================= */
const char* MQTT_TOPIC =
"glycowatch/devices/esp32-001/measurements";

/* =========================
   CLIENTS
========================= */
WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

/* =========================
   TIMERS
========================= */
unsigned long lastPublish = 0;
const long publishInterval = 5000;

/* =========================
   WIFI CONNECTION
========================= */
void connectWiFi() {

  Serial.println();
  Serial.println("==================================");
  Serial.println("CONECTANDO A WIFI...");
  Serial.println("==================================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WIFI CONECTADO");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

/* =========================
   MQTT CONNECTION
========================= */
void connectMQTT() {

  while (!mqttClient.connected()) {

    Serial.println();
    Serial.println("==================================");
    Serial.println("CONECTANDO A MQTT...");
    Serial.println("==================================");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    bool connected = mqttClient.connect(
      clientId.c_str(),
      MQTT_USERNAME,
      MQTT_PASSWORD
    );

    if (connected) {

      Serial.println("MQTT CONECTADO");
      Serial.print("BROKER: ");
      Serial.println(MQTT_BROKER);

    } else {

      Serial.print("ERROR MQTT -> rc=");
      Serial.println(mqttClient.state());

      Serial.println("REINTENTANDO EN 5 SEGUNDOS...");
      delay(5000);
    }
  }
}

/* =========================
   PUBLISH JSON
========================= */
void publishMeasurement() {

  StaticJsonDocument<256> doc;

  // Mantén aquí tu JSON actual
  doc["deviceId"] = "esp32-001";
  doc["glucose"] = random(80, 140);
  doc["heartRate"] = random(60, 100);
  doc["spo2"] = random(94, 100);
  doc["timestamp"] = millis();

  char payload[256];
  serializeJson(doc, payload);

  bool success = mqttClient.publish(MQTT_TOPIC, payload);

  if (success) {

    Serial.println();
    Serial.println("MEDICION PUBLICADA");
    Serial.print("TOPIC: ");
    Serial.println(MQTT_TOPIC);

    Serial.print("PAYLOAD: ");
    Serial.println(payload);

  } else {

    Serial.println();
    Serial.println("ERROR PUBLICANDO MEDICION");
  }
}

/* =========================
   SETUP
========================= */
void setup() {

  Serial.begin(115200);
  delay(1000);

  connectWiFi();

  /*
    IMPORTANTE:
    Para pruebas rápidas usamos:
  */
  secureClient.setInsecure();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

/* =========================
   LOOP
========================= */
void loop() {

  // Reconexión WiFi
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println();
    Serial.println("WIFI DESCONECTADO");
    connectWiFi();
  }

  // Reconexión MQTT
  if (!mqttClient.connected()) {

    connectMQTT();
  }

  mqttClient.loop();

  // Publicar medición cada 5 segundos
  unsigned long now = millis();

  if (now - lastPublish >= publishInterval) {

    lastPublish = now;

    publishMeasurement();
  }
}