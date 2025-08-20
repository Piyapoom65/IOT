#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

#define LINE_NOTIFY_TOKEN "1881u8Y5375n1umx1ph3Vy4FojJxOKo60mKeO1IzsNo" // Replace with your LINE Notify token
const int GPIOPIN = 2; // LED Pins
String etatGpio = "OFF"; // สถานะของ LED (ON/OFF)
ESP8266WebServer server(80); // สร้างเว็บเซิร์ฟเวอร์
WiFiManager wifiManager; // สร้างตัวจัดการ WiFi

//ฟังก์ชันการสร้างหน้าเว็บ
String getPage() { 
  String page = "<!DOCTYPE html><html lang='en'><head>";
  page += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  page += "<title>LED Control</title>";
  page += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/normalize/8.0.1/normalize.min.css'>";
  page += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/milligram/1.4.1/milligram.min.css'>";
  page += "<style>";
  page += "body {background-image: url('https://images.unsplash.com/photo-1557683304-673a23048d34');background-size: cover;background-repeat: no-repeat;background-attachment: fixed;display: flex;justify-content: center;align-items: center;height: 100vh;margin: 0;}";
  page += ".container { background-color: rgba(255, 255, 255, 0.8); padding: 20px; border-radius: 15px; text-align: center; width: 80%; max-width: 500px;}";
  page += "h1 { margin-bottom: 20px; }";
  page += ".badge { padding: 10px; margin-bottom: 20px; }";
  page += ".button {margin: 10px;}";
  page += "</style></head><body>";
  page += "<div class='container'>";
  page += "<h1>Controller for LED</h1>";
  page += "<div class='badge " + String(etatGpio == "ON" ? "badge-success" : "badge-danger") + "'>" + etatGpio + "</div>";
  page += "<form action='/' method='POST' style='display: flex; justify-content: space-around;'>";
  page += "<button type='submit' name='D" + String(GPIOPIN) + "' value='1' class='button button-outline'>ON</button>";
  page += "<button type='submit' name='D" + String(GPIOPIN) + "' value='0' class='button button-outline'>OFF</button>";
  page += "</form>";
  page += "<form action='/forget' method='POST' style='margin-top: 20px;'>";
  page += "<button type='submit' class='button button-outline'>Forget WiFi</button>";
  page += "</form>";
  page += "</div></body></html>";
  return page;
}

//ฟังก์ชัน URL หลัก
void handleRoot() { 
  if (server.hasArg("D2")) {
    handleGPIO(server.arg("D2"));
  } else {
    server.send(200, "text/html", getPage());
  }
}

//ฟังก์ชันจัดการการเปิด/ปิด LED
void handleGPIO(String value) {
  if (value == "1") {
    digitalWrite(GPIOPIN, LOW); // เปิด LED
    etatGpio = "ON";
    sendToLineNotify("LED is turned ON");
  } else if (value == "0") {
    digitalWrite(GPIOPIN, HIGH); // ปิด LED
    etatGpio = "OFF";
    sendToLineNotify("LED is turned OFF");
  }
  server.send(200, "text/html", getPage());
}

//ฟังก์ชันจัดการการลืมเครือข่าย WiFi
void handleForget() {
  wifiManager.resetSettings();
  server.send(200, "text/html", "<h1>WiFi settings have been reset. Please reboot the device.</h1>");
}

//ฟังก์ชันส่งแจ้งเตือนผ่าน LINE Notify
void sendToLineNotify(String message) {
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();
  http.begin(client, "https://notify-api.line.me/api/notify");

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(LINE_NOTIFY_TOKEN));

  String payload = "message=" + message;
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println(httpCode);
    Serial.println(response);
  } else {
    Serial.printf("Error in sending message: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void setup() {
  pinMode(GPIOPIN, OUTPUT);
  digitalWrite(GPIOPIN, HIGH); // ปิด LED เริ่มต้น

  Serial.begin(115200);

  // WiFiManager
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forget", HTTP_POST, handleForget);
  server.begin();
  Serial.println("HTTP server started");
}

//วนลูปเพื่อจัดการคำขอเว็บในฟังก์ชัน loop()
void loop() {
  server.handleClient();
}