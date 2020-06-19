#include "touch.h"

namespace touch
{

namespace
{
SPISettings spiSettings = SPISettings(TS_SPI_SPEED, MSBFIRST, SPI_MODE1);
}

TouchScreen::TouchScreen(uint8_t cspin, SPIClass *theSPI): Adafruit_STMPE610(cspin, theSPI)
{
    _CS = cspin;
    _spi = theSPI;
}

void TouchScreen::begin()
{
    pinMode(_CS, OUTPUT);
    digitalWrite(_CS, HIGH);
    delay(10);

    _spi->begin();
    delay(10);

    writeRegister(STMPE_SYS_CTRL1, STMPE_SYS_CTRL1_RESET);
    delay(10);

    for (uint8_t i = 0; i < 65; i++)
    {
        readRegister(i);
    }

    writeRegister(STMPE_SYS_CTRL2, 0x0);                                   // turn on clocks!
    writeRegister(STMPE_TSC_CTRL, STMPE_TSC_CTRL_XYZ | STMPE_TSC_CTRL_EN); // XYZ and enable!
    writeRegister(STMPE_INT_EN, STMPE_INT_EN_TOUCHDET);
    writeRegister(STMPE_ADC_CTRL1, STMPE_ADC_CTRL1_10BIT | (0x6 << 4)); // 96 clocks per conversion
    writeRegister(STMPE_ADC_CTRL2, STMPE_ADC_CTRL2_6_5MHZ);
    writeRegister(STMPE_TSC_CFG, STMPE_TSC_CFG_4SAMPLE | STMPE_TSC_CFG_DELAY_1MS | STMPE_TSC_CFG_SETTLE_5MS);
    writeRegister(STMPE_TSC_FRACTION_Z, 0x6);
    writeRegister(STMPE_FIFO_TH, 1);
    writeRegister(STMPE_FIFO_STA, STMPE_FIFO_STA_RESET);
    writeRegister(STMPE_FIFO_STA, 0); // unreset
    writeRegister(STMPE_TSC_I_DRIVE, STMPE_TSC_I_DRIVE_50MA);
    writeRegister(STMPE_INT_STA, 0xFF); // reset all ints
    writeRegister(STMPE_INT_CTRL, STMPE_INT_CTRL_POL_HIGH | STMPE_INT_CTRL_ENABLE);
}

uint8_t TouchScreen::readRegister(uint8_t reg)
{
    uint8_t x;

    _spi->beginTransaction(spiSettings);

    CS_LOW();
    _spi->transfer(0x80 | reg);
    _spi->transfer(0x00);
    x = _spi->transfer(0);
    CS_HIGH();

    _spi->endTransaction();

    return x;
}

void TouchScreen::writeRegister(uint8_t reg, uint8_t val)
{
    _spi->beginTransaction(spiSettings);

    CS_LOW();
    _spi->transfer(reg);
    _spi->transfer(val);
    CS_HIGH();

    _spi->endTransaction();
}

boolean TouchScreen::touched()
{
    return (readRegister(STMPE_TSC_CTRL) & 0x80);
}

boolean TouchScreen::bufferEmpty()
{
    return (readRegister(STMPE_FIFO_STA) & STMPE_FIFO_STA_EMPTY);
}

TouchPoint TouchScreen::getPoint()
{
    uint16_t x = -1, y = -1;
    uint8_t z = 0;
    uint8_t data[4];

    while (!bufferEmpty())
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            data[i] = readRegister(0xD7);
        }
        x = data[0];
        x <<= 4;
        x |= (data[1] >> 4);
        y = data[1] & 0x0F;
        y <<= 8;
        y |= data[2];
        z = data[3];
    }

    if (bufferEmpty())
    {
        writeRegister(STMPE_INT_STA, 0xFF); // reset all ints
    }

    return TouchPoint(x, y, z);
}

} // namespace touch