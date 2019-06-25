/*
 * PenguiOS v1.0 
 * 
 * Library requirements: 
 *  https://github.com/ThingPulse/esp8266-oled-ssd1306
 *  https://github.com/adafruit/Adafruit_NeoPixel
 * 
 */
#include <SimpleButton.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "SH1106.h"
#include "images.h"

using namespace simplebutton;

#define SDA_PIN 4
#define SCL_PIN 5

#define USE_WIFI false

int x, y, rndc;

int step_x = 1;
int step_y = 1;
unsigned long t0, t1;
float brightness = 0.1;

const char* ssid        = "SSID";
const char* password    = "PASSWORD";
const char* host_name   = "HOSTNAME";

/*
byte colors[] = { 
  0x00, 0x00, 0xff, //Blue
  0xff, 0xff, 0x00, //Yellow
  0xff, 0x00, 0xff, //Magenta 
  0xff, 0xa5, 0x00, //Orange
  0x99, 0x32, 0xcc  //Purple
};
*/
byte colors[] = { 
  0x00, 0x2f, 0xff,
  0xff, 0x21, 0x8d,
  0xcb, 0x22, 0xff,
  0xff, 0xec, 0x00,
  0xff, 0x88, 0x00
};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, 0, NEO_GRB + NEO_KHZ800);
SH1106 display(0x3c, SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println();

  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.clear();
  display.drawString(26, 30, "PenguiOS v1.0");
  display.display();
  display.display();

  strip.begin();
  strip.show();

  if (USE_WIFI) {
    WiFi.hostname(host_name);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.printf("Connecting to %s\n", ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Succesfully connected!");
    Serial.print("Host Name: ");
    Serial.println(WiFi.hostname());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.flush();

  randomSeed(os_random());
  rndc = random(0, 4);
  x = random(10, 100);
  y = random(10, 50);
  t0 = millis();
}

void loop() {
  t1 = millis();

  if (t1 - t0 > 25) {
    //collide with top wall
    if (y + step_y <= 0) {
        step_y *= -1;
        rndc = random(0, 4);
    }
    //collided with right wall
    if (x + 26 + step_x >= 128) {
        step_x *= -1;
        rndc = random(0, 4);
    }
    //collide with bottom wall
    if (y + 11 + step_y >= 64) {
        step_y *= -1;
        rndc = random(0, 4);
    }
    //collide with left wall
    if (x + step_x <= 0) {
        step_x *= -1;
        rndc = random(0, 4);
    }
    
    x += step_x;
    y += step_y;

    strip.setPixelColor(0, strip.Color(colors[rndc * 3] * brightness, colors[rndc * 3 + 1] * brightness, colors[rndc * 3 + 2] * brightness));
    strip.setPixelColor(1, strip.Color(colors[rndc * 3] * brightness, colors[rndc * 3 + 1] * brightness, colors[rndc * 3 + 2] * brightness));
    strip.show();
  
    display.clear();
    display.drawXbm(x, y, 32, 11, dvd);
    display.display();
    t0 = t1;
  }
}
