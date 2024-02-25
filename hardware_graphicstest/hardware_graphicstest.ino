#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//IMPORT IMAGE
#include "uwi.h"

#define TFT_DC    17
#define TFT_CS    5
#define TFT_RST   16
#define TFT_CLK   18
#define TFT_MOSI  23
#define TFT_MISO  19

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
int value =1000;

void setup(){
  Serial.begin(115200); 
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);
  tft.drawRGBBitmap(0,0, uwi, 240, 320); // DRAW IMAGE ON SCREEN
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(2);
 
}
void loop(void) { 
  
  tft.fillRoundRect(10, 50, 70, 25, 5, ILI9341_BLACK);
  tft.setCursor(20, 55);
  tft.print(value);
  value++;
  delay(1000);
}
