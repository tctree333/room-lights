#include "Particle.h"
#include "neopixel.h"
#include "WebDuino.h"
#include "math.h"

SYSTEM_THREAD(ENABLED)

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_COUNT 100
#define PIXEL_PIN D6
#define PIXEL_TYPE WS2812B

#define WEBDUINO_FAVICON_DATA ""

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

WebServer webserver("", 80);

uint8_t r = 0, g = 255, b = 255, brightness = 64;
String mode = "rainbow";

uint8_t rainbowJ = 255;
Timer rainbow_timer(300, rainbow);
void rainbow()
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, Wheel((i + rainbowJ) & 255));
  }
  strip.show();
  rainbowJ--; // rely on underflow to wrap back around
}

Timer random_timer(700, random_lights);
void random_lights()
{

  strip.setPixelColor(random(strip.numPixels() - 1), random(255), random(255), random(255));

  strip.show();
  random_timer.changePeriod(random(800) + 200);
}

Timer random_around_timer(300, random_around);
void random_around()
{
  strip.setPixelColor(random(strip.numPixels() - 1), g ^ random(64), r ^ random(64), b ^ random(64));
  strip.show();
  random_around_timer.changePeriod(random(500) + 200);
}

void color(uint8_t r, uint8_t g, uint8_t b)
{
  rainbow_timer.stop();
  random_timer.stop();
  random_around_timer.stop();

  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, g, r, b);
  }
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void setLights(String data)
{
  if (data == "on")
  {
    setLights(mode);
  }
  else if (data == "off")
  {
    color(0, 0, 0);
  }
  else if (data == "rainbow")
  {
    mode = "rainbow";

    random_timer.stop();
    random_around_timer.stop();
    if (!rainbow_timer.isActive())
    {
      rainbow_timer.start();
    }
  }
  else if (data == "pureRandom")
  {
    mode = "pureRandom";

    rainbow_timer.stop();
    random_around_timer.stop();

    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, random(255), random(255), random(255));
    }
    strip.show();

    if (!random_timer.isActive())
    {
      random_timer.start();
    }
  }
  else if (data == "randomAround")
  {
    mode = "randomAround";

    rainbow_timer.stop();
    random_timer.stop();

    for (uint16_t i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, g ^ random(64), r ^ random(64), b ^ random(64));
    }
    strip.show();

    if (!random_around_timer.isActive())
    {
      random_around_timer.start();
    }
  }
  else if (data == "manual")
  {
    mode = "manual";

    color(r, g, b);
  }
  else if (data.length() == 3 && data[0] == 66) // compare to "B"
  {
    data.remove(0, 1);

    uint32_t number = strtol(data, NULL, 16);
    brightness = number & 0xFF;

    strip.setBrightness(brightness);
    strip.show();
  }
  else if (data.length() == 7 && data[0] == 36) // compare to "$"
  {
    data.remove(0, 1);

    uint32_t number = strtol(data, NULL, 16);

    // Split them up into r, g, b values
    r = number >> 16;
    g = number >> 8 & 0xFF;
    b = number & 0xFF;

    data = "$" + data;
  }
}
void command(WebServer &server, WebServer::ConnectionType type, char *tail, bool tail_complete)
{
  server.httpSuccess("text/plain");

  if (type == WebServer::HEAD)
    return;

  setLights(String(tail));

  server.printP("ok");
}

void statusCommand(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  server.httpSuccess("application/json");

  if (type == WebServer::HEAD)
    return;

  String status = "{\"color\": " +
                  String(r << 16 | g << 8 | b) +
                  ", \"brightness\": " +
                  String(brightness) +
                  ", \"mode\": \"" +
                  mode +
                  "\"}";

  server.printP(status);
}

void setup()
{
  strip.begin();
  strip.setBrightness(brightness);
  rainbow_timer.start();

  waitFor(WiFi.ready, 10000);
  webserver.setDefaultCommand(&command);
  webserver.addCommand("status", &statusCommand);
  webserver.begin();
}

void loop()
{
  char buff[64];
  int len = 64;

  webserver.processConnection(buff, &len);
}