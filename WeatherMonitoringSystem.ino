
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_BMP280.h>

// Define Wi-Fi credentials and ThingSpeak API
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";
String thingSpeakAPIKey = "YOUR_THINGSPEAK_API_KEY";
String thingSpeakServer = "https://api.thingspeak.com/update";

// Sensor pins and constants
#define DHTPIN 4              // GPIO4 for DHT11/DHT22 data pin
#define DHTTYPE DHT22         // DHT sensor type (DHT11 or DHT22)
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;          // I2C BMP280 sensor

// Wi-Fi connection setup
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

// Initialize all sensors
void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWiFi();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

// Function to upload data to ThingSpeak
void uploadData(float temp, float hum, float pressure) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = thingSpeakServer + "?api_key=" + thingSpeakAPIKey
                 + "&field1=" + String(temp)
                 + "&field2=" + String(hum)
                 + "&field3=" + String(pressure);

    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully!");
    } else {
      Serial.println("Error sending data.");
    }
    http.end();
  }
}

void loop() {
  // Read DHT sensor data
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Read pressure data from BMP280
  float pressure = bmp.readPressure() / 100.0F;  // Convert to hPa

  // Print values
  Serial.print("Temp: "); Serial.print(temp); Serial.print("°C ");
  Serial.print("Humidity: "); Serial.print(hum); Serial.print("% ");
  Serial.print("Pressure: "); Serial.print(pressure); Serial.println(" hPa");

  // Send data to ThingSpeak
  uploadData(temp, hum, pressure);

  // Wait 15 seconds before the next reading
  delay(15000);
}
