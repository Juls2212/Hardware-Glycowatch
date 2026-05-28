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

/* =========================
   MQTT TOPIC
========================= */
const char* MQTT_TOPIC =
"glycowatch/devices/esp32-001/measurements";

/* =========================
   DEVICE DATA
========================= */
const char* deviceIdentifier = "ESP32-111";
const char* apiKey = "fNnCRHetsPSiViBohecT4Hawu7E7eKpjqaq-w78vPSM";

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

int counter = 0;

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

void publishMeasurement() {
  counter++;

  int glucose;
  int tipo = counter % 4;

  if (tipo == 0 || tipo == 1) {
    glucose = random(80, 200);
  } else if (tipo == 2) {
    glucose = random(200, 500);
  } else {
    glucose = random(50, 79);
  }

  String sourceEventId = "esp32-" + String(counter);
  String tiempoActual = obtenerTiempoISO();

  String payload = "{";
  payload += "\"deviceIdentifier\":\"" + String(deviceIdentifier) + "\",";
  payload += "\"apiKey\":\"" + String(apiKey) + "\",";
  payload += "\"glucoseMgDl\":" + String(glucose) + ",";
  payload += "\"measuredAt\":\"" + tiempoActual + "\",";
  payload += "\"sourceEventId\":\"" + sourceEventId + "\",";
  payload += "\"origin\":\"esp32-mqtt\"";
  payload += "}";

  Serial.println();
  Serial.println("PAYLOAD GENERADO:");
  Serial.println(payload);
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

  publishMeasurement();

  Serial.println();
  Serial.println("==================================");
  Serial.println("ESP32 LISTO!");
  Serial.println("==================================");
}

void loop() {
  mqttClient.loop();
}