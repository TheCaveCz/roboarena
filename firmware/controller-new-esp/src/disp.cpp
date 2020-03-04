#include "disp.h"
#include <ssd1306.h>
#include <Wire.h>
#include <SPI.h>

void dispSetup(Scheduler *scheduler) {
  #if LOGIC_REMOTE
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_flipHorizontal(1);
  ssd1306_flipVertical(1);

  ssd1306_clearScreen();
  //ssd1306_drawRect(0,0,ssd1306_displayWidth()-1,ssd1306_displayHeight()-1);
  #endif
}

