#include <WiFi.h>
#include <PubSubClient.h>
#include "time.h"

// WIFI
const char* ssid = "ESCOBAR-2G";
const char* password = "WIFI-PASSWORD";

// MQTT
const char* mqtt_server = "test.mosquitto.org";
const char* topic = "glycowatch/device/ESP32-ABC-123";

// DEVICE DATA
const char* deviceIdentifier = "ESP32-015";
const char* apiKey = "zMi8qrn1MO07U-a200g6E6fct8KzTOPNb1QqVZJdVUI";

// BOTÓN (BOOT)
#define BUTTON_PIN 0

WiFiClient espClient;
PubSubClient client(espClient);

// contador
int counter = 0;

// Hora actual 
String obtenerTiempoISO() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "2026-01-01T00:00:00Z"; // fallback
  }

  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buffer);
}

void setup_wifi() {
  Serial.println("Conectando a WiFi...");
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

// ENVIAR DATOS
void enviarMedicion() {
  counter++;

  int glucose;
  int tipo = counter % 4;

  if (tipo == 0 || tipo == 1) {
    glucose = random(80, 200); //Normal
  } else if (tipo == 2) {
    glucose = random(200, 500); //Alta
  } else {
    glucose = random(50, 79); //Baja
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

  client.publish(topic, payload.c_str(), false);


  Serial.println("Enviado:");
  Serial.println("{");
  Serial.println("  \"deviceIdentifier\": \"" + String(deviceIdentifier) + "\",");
  Serial.println("  \"apiKey\": \"" + String(apiKey) + "\",");
  Serial.println("  \"glucoseMgDl\": " + String(glucose) + ",");
  Serial.println("  \"measuredAt\": \"" + tiempoActual + "\",");
  Serial.println("  \"sourceEventId\": \"" + sourceEventId + "\",");
  Serial.println("  \"origin\": \"esp32-mqtt\"");
  Serial.println("}");
  Serial.println("----------------------");
}

void setup() {
  Serial.begin(115200);

  setup_wifi();

  // Sincronizar hora
  configTime(0, 0, "pool.ntp.org");

  client.setServer(mqtt_server, 1883);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // BOTÓN
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_PIN);

  if (currentState == LOW && lastState == HIGH) {
    Serial.println("Botón presionado 🚀");
    enviarMedicion();
    delay(300);
  }

  lastState = currentState;

  // Envio automatico sin bloqueo
  static unsigned long lastSend = 0;

  if (millis() - lastSend > 1200000) {
    enviarMedicion();
    lastSend = millis();
  }
}