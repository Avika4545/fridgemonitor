#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char* ssid = "TemperatureApp";
const char* password = "12345678";

float temperature = 0.0;
float humidity = 0.0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  Wire.begin(25, 26);

  dht.begin();
  lcd.init();
  lcd.backlight();

  bool apSuccess = WiFi.softAP(ssid, password);
  if (apSuccess) {
    Serial.println("Access Point started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Access Point failed to start!");
  }

  server.begin();

  lcd.setCursor(0, 0);
  lcd.print("WiFi: ESP32_Temp");
  lcd.setCursor(0, 1);
  lcd.print("IP:");
  lcd.print(WiFi.softAPIP());
  delay(3000);
  lcd.clear();
}

void loop() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  if (isnan(temperature)) lcd.print("Err");
  else lcd.print(temperature, 1);

  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  if (isnan(humidity)) lcd.print("Err");
  else lcd.print(humidity, 1);
  lcd.print("%");

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");

    String response = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>ESP32 Temp</title>";
    response += "<meta http-equiv='refresh' content='5'></head><body>";
    response += "<h2>ESP32 Temperature Monitor</h2>";
    if (!isnan(temperature)) {
      response += "<p>Temperature: " + String(temperature, 1) + " &deg;C</p>";
    } else {
      response += "<p>Temperature: Sensor Error</p>";
    }
    response += "</body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println(response);
    client.stop();
    Serial.println("Client Disconnected");
  }

  delay(2000);
}
