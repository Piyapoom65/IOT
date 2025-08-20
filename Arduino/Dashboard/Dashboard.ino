#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <math.h>

const char* serverName = "http://172.20.10.6/sensordata/post-esp-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "BMP280";
String sensorLocation = "Chiang Mai";
// ตั้งค่า Thingspeak

WiFiManager wifiManager;
int takedown = 0 ;
// สร้างออบเจกต์สำหรับ BMP280
Adafruit_BMP280 bmp;

void readSensorAndSend() {
  Serial.println("Reading sensor and sending data...");

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // อ่านค่าจากเซ็นเซอร์และส่งข้อมูลไปยัง ThingSpeak
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F;
  float altitude = bmp.readAltitude(1013.25);

  if (isnan(temperature) || isnan(pressure)) {
    Serial.println("Failed to read from BMP280 sensor!");
    return;
  }

  pressure = fabs(pressure);

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Pressure: ");
  Serial.println(pressure);
  Serial.print("Altitude: ");
  Serial.println(altitude);

  sendToThingSpeak(temperature, pressure, altitude);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");
  delay(8000) ;
  Wire.begin();
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  takedown++;
  connectWiFi();
  readSensorAndSend();
  Serial.println("Going to sleep for 1 minutes...");
  ESP.deepSleep(600e5); // Sleep for 10 minutes
  // ESP.deepSleep(10 * 60 * 100);
  // อ่านค่าจากเซ็นเซอร์และส่งข้อมูลครั้งแรกทันที
}

void connectWiFi() {
  Serial.print("Connecting.... ");

  int attempts = 0;
  wifiManager.setConnectTimeout(10);
  wifiManager.setConnectRetries(45);
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    wifiManager.autoConnect("AO WIFIMANAGER");
    Serial.print("Attemp =");
    Serial.println(attempts);
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi");
  }
}

void sendToThingSpeak(float temperature, float pressure, float altitude) {
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    WiFiClient client;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client,serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&value1=" + String(bmp.readTemperature())
+ "&value2=" + String(bmp.readAltitude(1013.25)) + "&value3=" + String(bmp.readPressure()/100.0F) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&location=Office&value1=24.75&value2=49.54&value3=1005.14";
 // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void loop() {
  Serial.print("Try again in 10 minutes") ;
  if (takedown == 0){
    ESP.reset ;
  }
  else{
    ESP.deepSleep(600e5); // Sleep for 10 minutes
  }
}