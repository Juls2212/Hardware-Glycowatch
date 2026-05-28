#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

/* =========================
   WIFI CONFIG
========================= */
const char* WIFI_SSID = "WIFI_UCC_ESTUDIANTES";
const char* WIFI_PASSWORD = "Estudiantes.2026*";

/* =========================
   HIVEMQ CLOUD CONFIG
========================= */
const char* MQTT_BROKER =
"478aa6d5a50348a285de276f76d74233.s1.eu.hivemq.cloud";

const int MQTT_PORT = 8883;

const char* MQTT_USERNAME = "glycowatch_backend";
const char* MQTT_PASSWORD = "glycoProyecto2212";

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

String obtenerTiempoISO() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    Serial.println("ERROR OBTENIENDO HORA");
    return "2026-01-01T00:00:00-05:00";
  }

  char buffer[35];

  strftime(
    buffer,
    sizeof(buffer),
    "%Y-%m-%dT%H:%M:%S-05:00",
    &timeinfo
  );

  return String(buffer);
}

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
  Serial.println("WIFI CONECTADO!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

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
      Serial.println("MQTT CONECTADO!");
      Serial.print("BROKER: ");
      Serial.println(MQTT_BROKER);
    } else {
      Serial.print("ERROR MQTT rc=");
      Serial.println(mqttClient.state());

      Serial.println("REINTENTANDO EN 5 SEGUNDOS...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  connectWiFi();

  configTime(
    -5 * 3600,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  secureClient.setInsecure();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  connectMQTT();

  Serial.println();
  Serial.println("==================================");
  Serial.println("ESP32 LISTO!");
  Serial.println("==================================");
}

void loop() {
  mqttClient.loop();
}