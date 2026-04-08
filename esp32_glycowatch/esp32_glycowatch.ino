<<<<<<< HEAD
=======
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ESCOBAR-2G";
const char* password = "WIFIPASSWORD";

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

>>>>>>> 7833e3c (ESP32 .INO FUNCIONANDO)
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("ESP32 funcionando 🔥");
  delay(1000);
}