/* 
 * File:   tft.h
 * Author: Adam Martini
 *
 * Created on May 3, 2020, 10:18 PM
 */

#ifndef TFT_H
#define TFT_H

#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_HX8357.h>

namespace tft
{

#define TFT_SPI_SPEED 24000000
#define TFT_CS 9
#define TFT_DC 10

#define CS_LOW()          \
  digitalWrite(_CS, LOW); \
  digitalWrite(_CS, LOW); \
  digitalWrite(_CS, LOW);
#define CS_HIGH() digitalWrite(_CS, HIGH);

#define DC_LOW()          \
  digitalWrite(_DC, LOW); \
  digitalWrite(_DC, LOW); \
  digitalWrite(_DC, LOW);
#define DC_HIGH() digitalWrite(_DC, HIGH);

class TFT : public Adafruit_HX8357
{
public:
  TFT(int8_t cs, int8_t dc);

  void begin();
  void writePixelsDMA(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t *colors, uint32_t len);
};

} // namespace tft

#endif /* TFT_H */