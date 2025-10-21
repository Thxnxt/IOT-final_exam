#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Adafruit_SHT31.h"

// -------------------------------
// ‼️‼️ แก้ไข 3 บรรทัดนี้ ‼️‼️
// -------------------------------
const char* ssid = "";       // <<! 1. ใส่ชื่อ WiFi Hotspot
const char* password = "";   // <<! 2. ใส่รหัสผ่าน Hotspot
const char* topic = "66070084/final_test";    // <<! 3. ตั้งชื่อ Topic (ต้องตรงกับ Pi)

// -------------------------------
// (ส่วนนี้ใช้ตามโค้ดคุณได้เลย)
const char* mqtt_server = "mqtt-dashboard.com";
const int mqtt_port = 1883;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// -------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1000);

  // เริ่มต้น SHT31
  if (!sht31.begin(0x44)) { // 0x44 คือ I2C address มาตรฐาน
    Serial.println("Couldn't find SHT31 sensor!");
    while (1);
  }

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // ตั้งค่า MQTT
  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ArduinoR4Client")) { // (ตั้งชื่อ Client ID ของคุณ)
      Serial.println("connected!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      delay(3000);
    }
  }
}

// -------------------------------
void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // อ่านค่าอุณหภูมิและความชื้น
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    // สร้าง payload JSON สำหรับส่งผ่าน MQTT
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"temp\": %.2f, \"humid\": %.2f}", t, h);
    
    // ส่งขึ้น MQTT
    client.publish(topic, payload);
    Serial.print("Published: ");
    Serial.println(payload);
  } else {
    Serial.println("Failed to read from SHT31 sensor!");
  }

  delay(5000); // ส่งข้อมูลทุก 5 วินาที
}
