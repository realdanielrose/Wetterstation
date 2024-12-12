/*
 * ESP8266 Board Information:
 * 
 * Board: NodeMCU 1.0 (ESP-12E Module)
 * - Processor: 32-bit RISC CPU running at 80 MHz (can be overclocked to 160 MHz)
 * - Memory: 32 KB instruction RAM, 80 KB user data RAM, 16 KB ETS system data RAM, built-in flash memory
 * - GPIO: 16 GPIO pins, PWM output, I2C, SPI, and UART interfaces
 * - Wi-Fi: Supports 2.4 GHz Wi-Fi (802.11 b/g/n), full TCP/IP protocol stack
 * - Other: ADC (Analog to Digital Converter) with 10-bit resolution
 * 
 * Ensure the correct board is selected in the Arduino IDE:
 * - Board: NodeMCU 1.0 (ESP-12E Module)
 * - Port: Select the appropriate COM port
 * - Flash Size: 4M (3M SPIFFS)
 * - Upload Speed: 115200
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "eis.h"
#include "sun.h"
#include "moon.h"
#include "cloud.h"
#include "drop.h"

#define DHT_TYPE DHT11
#define DHT_PIN D5
#define DHT_POWER D0
#define LDRPIN A0

const char* ssid = "FRITZ!Box Beatmaker";
const char* password = "password"; 

// DHT-Sensor-Objekt
DHT dht(DHT_PIN, DHT_TYPE);

// OLED-Display
Adafruit_SSD1306 display(128, 64, &Wire, 0x3C);

// Zeit-Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 7200, 60000);

// Wochentage
String weekdays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup() {
  // OLED initialisieren
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // WiFi verbinden
  Serial.begin(115200);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Connecting to WiFi...");
  display.display();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // DHT-Sensor und NTP-Client initialisieren
  pinMode(DHT_POWER, OUTPUT);
  digitalWrite(DHT_POWER, HIGH);
  dht.begin();
  timeClient.begin();
  timeClient.update();

  // Begrüßung anzeigen
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("WiFi connected!");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Daten erfassen
  int ldr = analogRead(LDRPIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  timeClient.update();

  // Display aktualisieren
  display.clearDisplay();

  // Überschrift
  display.setCursor(10, 0);
  display.setTextSize(2);
  display.println("Weather");

  // Temperatur
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  // Luftfeuchtigkeit
  display.setCursor(0, 30);
  display.print("Hum: ");
  display.print(humidity);
  display.println(" %");

  // Helligkeit
  display.setCursor(0, 40);
  display.print("Light: ");
  display.print(ldr);
  display.println(" L");

  // Symbol abhängig von der Helligkeit
  if (ldr < 500) {
    display.drawXBitmap(90, 20, moon_bits, moon_width, moon_height, WHITE);
  } else {
    display.drawXBitmap(90, 20, sun_bits, sun_width, sun_height, WHITE);
  }

  // Aktualisieren
  display.display();
  delay(2000);
  fadeOut();

  // Zeit anzeigen
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Day: ");
  display.println(weekdays[timeClient.getDay()]);
  display.setCursor(0, 10);
  display.print("Time: ");
  display.println(timeClient.getFormattedTime());
  display.display();
  delay(2000);
  fadeOut();
}

void fadeOut() {
  for (int contrast = 255; contrast >= 0; contrast -= 5) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(contrast);
    delay(30);
  }
  display.clearDisplay();
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(255);
}
