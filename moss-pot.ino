#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <WS2812FX.h>

#define LED_COUNT 7
#define LED_PIN 12

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define DHTPIN 3         // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  startupText();

  ws2812fx.init();
  ws2812fx.setBrightness(65);
  ws2812fx.setSpeed(200);
  ws2812fx.setColor(WHITE);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
  ws2812fx.service();
  
  // Sensor
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  delayMS = sensor.min_delay / 1000; 

  //display.setRotation(2);
  display.display();
}

void startupText() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setCursor(0, 16);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.write("   Initializing...   ");
  display.display();
}


void renderInfo() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Temp
  display.setCursor(0, 0);     // Start at top-left corner

  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    display.write("Cannot read temp...");
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setTextSize(3);
    display.setCursor(2, 10);
    display.write("ERROR");
    display.display();
    delay(2000);
  }

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.write(" Temperature:       ");

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setTextSize(3);
  display.setCursor(2, 10);
  String t = String(event.temperature)+String(" C");
  display.write(t.c_str());
  
  display.display();
  delay(2000);

  // Humidity
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0, 0);     // Start at top-left corner

  delay(delayMS);
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    display.write("Cannot read humidity");
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    display.setTextSize(3);
    display.setCursor(2, 10);
    display.write("ERROR");
    display.display();
    delay(2000);
  }

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.write(" Humidity:          ");

  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setTextSize(3);
  display.setCursor(2, 10);
  String t2 = String(event.relative_humidity)+String(" %");
  display.write(t2.c_str());
  
  display.display();
  delay(2000);
  
}

void loop() {
  renderInfo();
  ws2812fx.service();
}
