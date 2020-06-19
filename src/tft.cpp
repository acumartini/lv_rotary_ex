#include "tft.h"

namespace tft
{

namespace
{
int8_t _CS;
SPIClass *_spi;

volatile bool dma_colors_busy = false;
}

void tft_dma_colors_callback(Adafruit_ZeroDMA *dma)
{
    digitalWrite(_CS, HIGH);
    _spi->endTransaction();

    dma_colors_busy = false;
}

void tft_dma_colors_errorback(Adafruit_ZeroDMA *dma)
{
    Serial.print("DMA TRANSFER ERROR [TFT]: ");
    dma->printStatus();
    while (true)
        ;
}

TFT::TFT(int8_t cs, int8_t dc) : Adafruit_HX8357(cs, dc)
{
    _CS = cs;
}

void TFT::begin()
{
    Adafruit_HX8357::begin();
    
    setSPISpeed(TFT_SPI_SPEED);
    _spi = hwspi._spi;

    // dma.allocate();
    // dma.setCallback(tft_dma_colors_callback, DMA_CALLBACK_TRANSFER_DONE);
    // dma.setCallback(tft_dma_colors_errorback, DMA_CALLBACK_TRANSFER_ERROR);

    // dptr->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
}

/**
 * Note: this method assumes colors are big-endian
 */
void TFT::writePixelsDMA(uint8_t x1, uint8_t y1, uint8_t w, uint8_t h, uint16_t *colors, uint32_t len)
{
    if (!len)
        return; // avoid 0-byte transfers

    while (dma_colors_busy)
        ;
    dma_colors_busy = true;

    startWrite();
    setAddrWindow(x1, y1, w, h);

    dptr->SRCADDR.reg = (uint32_t)colors;
    dptr->BTCTRL.bit.SRCINC = 1;
    dptr->BTCNT.reg = len * 2;
    dptr->DESCADDR.reg = 0;

    // PORT->Group[0].OUTSET.reg = 0x800000; //led pin 13 set high; PinHigh(); out of scope
    dma.startJob();
}

} // namespace tft