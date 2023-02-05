/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_GFX.h"
#include <Adafruit_ILI9341.h>
#include "Adafruit_Si7021.h"
#include <Wire.h>
#include "Adafruit_STMPE610.h"

bool enableHeater = false;
uint8_t loopCnt = 0;
Adafruit_Si7021 sensor = Adafruit_Si7021();

bool Blink = true;

#define STMPE_SCK 10
#define STMPE_CS 9
#define STMPE_SDI 11
#define STMPE_SDO 8

Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS, STMPE_SDI, STMPE_SDO, STMPE_SCK);
// Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

//Adafruit_FT6206 ctp = Adafruit_FT6206();

// For the Adafruit shield, these are the default.
#define TFT_DC 15
#define TFT_CS 13
#define TFT_MOSI 7
#define TFT_CLK 6
#define TFT_RST 14
#define TFT_MISO 4

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define WIRE Wire

void setup() {
  // Serial Monitor
  Serial.begin(115200);
  Serial.flush();
  // wait for serial port to open
  // while (!Serial) {
  //   delay(10);
  // }

  // Temp sensor Si7021
  Serial.println("Si7021 test!");

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  Serial.print("Found model ");
  switch (sensor.getModel()) {
    case SI_Engineering_Samples:
      Serial.print("SI engineering samples");
      break;
    case SI_7013:
      Serial.print("Si7013");
      break;
    case SI_7020:
      Serial.print("Si7020");
      break;
    case SI_7021:
      Serial.print("Si7021");
      break;
    case SI_UNKNOWN:
    default:
      Serial.print("Unknown");
  }

  Serial.print(" Rev(");
  Serial.print(sensor.getRevision());
  Serial.print(")");
  Serial.print(" Serial #");
  Serial.print(sensor.sernum_a, HEX);
  Serial.println(sensor.sernum_b, HEX);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(22u, OUTPUT);

  // ILI9341 TFT
  tft.begin();

  tft.setCursor(0, 0);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setRotation(3);
  tft.println("Hello World!");

  Serial.println("ILI9341 Test!");
  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x");
  Serial.println(x, HEX);

  WIRE.begin();
  I2CScanner();

  Serial.println(F("Done!"));
}

void setup1()
{
    // If using I2C you can select the I2C address (there are two options) by calling
  // touch.begin(0x41), the default, or touch.begin(0x44) if A0 is tied to 3.3V
  // If no address is passed, 0x41 is used
  if (!touch.begin()) {
    Serial.println("STMPE not found!");
    // while(1);
  }
  Serial.println("Waiting for touch sense");
}

void loop(void) {
  //  for(uint8_t rotation=0; rotation<4; rotation+
  if (Blink) {
    // tft.println("On");
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(22u, HIGH);
  } else {
    // tft.println("Off");
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    digitalWrite(22u, LOW);
  }
  Blink = !Blink;

  tft.setCursor(0, 50);
  tft.printf("Core temperature: %2.1fC\n", analogReadTemp());

  tft.setCursor(0, 100);
  tft.print("Humidity:    ");
  tft.println(sensor.readHumidity(), 2);
  tft.print("Temperature: ");
  tft.println(sensor.readTemperature(), 2);

  // Toggle heater enabled state every 30 seconds
  // An ~1.8 degC temperature increase can be noted when heater is enabled
  if (++loopCnt == 30) {
    enableHeater = !enableHeater;
    sensor.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    if (sensor.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    loopCnt = 0;
  }

  delay(100);
}

void loop1()
{
#define Test
#ifdef Test
  uint16_t x, y;
  uint8_t z;
  if (touch.touched()) {
    // read x & y & z;
    while (!touch.bufferEmpty()) {
      Serial.print(touch.bufferSize());
      touch.readData(&x, &y, &z);
      Serial.print("->(");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(", ");
      Serial.print(z);
      Serial.println(")");
    }
    touch.writeRegister8(STMPE_INT_STA, 0xFF);  // reset all ints, in this example unneeded depending in use
  }
#endif  
}

unsigned long I2CScanner() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmission to see if
    // a device did acknowledge the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  return 0;
}
