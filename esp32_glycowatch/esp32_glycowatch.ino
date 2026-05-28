#include <WiFi.h>

/* =========================
   WIFI CONFIG
========================= */
const char* WIFI_SSID = "WIFI_UCC_ESTUDIANTES";
const char* WIFI_PASSWORD = "Estudiantes.2026*";

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

void setup() {
  Serial.begin(115200);

  connectWiFi();
}

void loop() {

}