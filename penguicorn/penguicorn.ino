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
#include "fonts.h"

using namespace simplebutton;

#define SDA_PIN 4
#define SCL_PIN 5

#define USE_WIFI false

int frame, wheelIt, pushTime, ledMode;
float brightness;
boolean increment;
unsigned long t0;
Button* b;

const char* ssid      = "SSID";
const char* password  = "PASSWORD";
const char* host_name  = "HOSTNAME";

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

  increment = true;
  brightness = 0.05;
  ledMode = 0;
  frame = 0;
  wheelIt = 0;
  b = new ButtonPullup(1);
  t0 = 0;
}

void loop() {
  if (ledMode == 0) {
    if (t0 >= 1000) {
      strip.setPixelColor(0, Wheel((0 + wheelIt) & 255));
      strip.setPixelColor(1, Wheel((1 + wheelIt) & 255));
      strip.show();

      wheelIt++;
      if (wheelIt == 256)
        wheelIt = 0;
      t0 = 0;
    } else {
      t0++;
    }
  } else if (ledMode == 1) {
    if (t0 > 800) {
      strip.setPixelColor(0, strip.Color(255 * brightness, 0, 0));
      strip.setPixelColor(1, strip.Color(255 * brightness, 0, 0));
      strip.show();
      if (increment) {
        brightness += 0.01;
      } else {
        brightness -= 0.01;
      }

      if (brightness <= 0.05) {
        increment = true;
      }
      if (brightness >= 0.95) {
        increment = false;
      }
      t0 = 0;
    } else {
      t0++;
    }
  }

  b->update();
  if (b->holding()) {
    display.normalDisplay();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    pushTime = b->getPushTime();
    if (pushTime >= 1000 && pushTime <= 2000) {
      display.clear();
      display.drawString(64, 22, "Rainbow/Evil Mode");
      display.display();
    } else if (pushTime >= 2000 && pushTime <= 3000) {
      display.clear();
      display.drawString(64, 22, "Smooth/Eye Fucking");
      display.display();
    } else if (pushTime > 3000) {
      display.clear();
      display.drawString(64, 22, "Sleep ma boi!");
      display.display();
    }

  }

  if (b->released()) {
    pushTime = b->getPushTime();

    if (pushTime <= 500) {
      frame++;
    } else if (pushTime >= 1000 && pushTime <= 2000) {
      //Change Led Mode
      ledMode++;
      if (ledMode == 0) {
        brightness = 0.05;
      } else if (ledMode == 1) {
        brightness = 0.2;
      } else if (ledMode >= 2) {
        ledMode = 0;
        brightness = 0.05;
      }
    } else if (pushTime >= 2000 && pushTime <= 3000) {
      if (brightness == 1) {
        brightness = 0.05;
      } else {
        brightness = 1;
      }
    } else if (pushTime > 3000) {
      ledMode = -1;
      frame = 0;
      brightness = 0;
      strip.setPixelColor(0, strip.Color(0, 0, 0));
      strip.setPixelColor(1, strip.Color(0, 0, 0));
      strip.show();
    }

    if (frame > 5)
      frame = 1;
    loadFrame(frame);
  }
}

void loadFrame(int id) {
  display.normalDisplay();
  display.clear();
  switch (id) {
    case 0:
      display.display();
      break;
    case 1:
      //display.drawXbm(0, 0, 112, 57, chaos);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(Roboto_Condensed_48);
      display.drawString(64, 0, "#35C3");
      display.setFont(ArialMT_Plain_10);
      display.drawString(64, 52, "InnSecure Assembly");
      display.display();
      break;
    case 2:
      display.drawXbm(28, 0, 64, 59, heart);
      display.display();
      break;
    case 3:
      display.drawXbm(32, 0, 64, 63, dickbutt);
      display.display();
      break;
    case 4:
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_16);
      display.drawString(64, 12, "@thepenguicorn");
      display.setFont(ArialMT_Plain_10);
      display.drawString(64, 34, "InnSecure Assembly");
      display.display();
      break;
    case 5:
      display.invertDisplay();
      display.drawXbm(42, 4, 48, 42, twitterqr);
      display.setFont(ArialMT_Plain_10);
      display.drawString(64, 50, "@thepenguicorn");
      display.display();
      break;
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3) * brightness, 0, (WheelPos * 3) * brightness);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (WheelPos * 3) * brightness, (255 - WheelPos * 3) * brightness);
  }
  WheelPos -= 170;
  return strip.Color((WheelPos * 3) * brightness, (255 - WheelPos * 3) * brightness, 0);
}
