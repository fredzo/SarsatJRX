/////////////////////////////////////////////////////////////////
/*
MIT License

Copyright (c) 2019 lewis he

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

focaltech.cpp - Arduino library for focaltech chip, support FT5206,FT6236,FT5336,FT6436L,FT6436
Created by Lewis on April 17, 2019.
github:https://github.com/lewisxhe/FocalTech_Library
*/
/////////////////////////////////////////////////////////////////
#include "gt911.h"


/////////////////////GT9XX//////////////////////////////////////////////////
/////////////////////GT9XX//////////////////////////////////////////////////
/////////////////////GT9XX//////////////////////////////////////////////////
/////////////////////GT9XX//////////////////////////////////////////////////
#define GT9XX_COORDINATE            (0x814E)
#define GT9XX_CLEARBUF              (0x814E)
#define GT9XX_CONFIG                (0x8047)
#define GT9XX_COMMAND               (0x8040)
#define GT9XX_PRODUCT_ID            (0x8140)
#define GT9XX_VENDOR_ID             (0x814A)
#define GT9XX_CONFIG_VERSION        (0x8047)
#define GT9XX_CONFIG_CHECKSUM       (0x80FF)
#define GT9XX_FIRMWARE_VERSION      (0x8144)


bool GT9xx_Class::probe(void)
{
    uint8_t config0[] = {
        0x5D, 0x40, 0x01, 0xE0, 0x01, 0x05, 0x35, 0x00, 0x01, 0x08,
        0x1E, 0x0F, 0x50, 0x32, 0x03, 0x05, 0x00, 0x00, 0x00, 0x00,
        0x22, 0x22, 0x00, 0x18, 0x1B, 0x1E, 0x14, 0x87, 0x27, 0x0A,
        0x3C, 0x3E, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x9B, 0x02, 0x1C,
        0x00, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x00,
        0x00, 0x28, 0x6E, 0x94, 0xC5, 0x02, 0x00, 0x00, 0x00, 0x04,
        0xAB, 0x2C, 0x00, 0x8D, 0x36, 0x00, 0x75, 0x42, 0x00, 0x61,
        0x51, 0x00, 0x51, 0x63, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00,
        0xF0, 0x4A, 0x3A, 0xFF, 0xFF, 0x27, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x14, 0x12, 0x10, 0x0E, 0x0C, 0x0A, 0x08, 0x06,
        0x04, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x24,
        0x22, 0x21, 0x20, 0x1F, 0x1E, 0x1D, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xC0, 0x01
    };

    // if (_rst > 0) {
    //     pinMode(_rst, OUTPUT);
    //     if (_interrupt > 0)
    //         pinMode(_interrupt, OUTPUT);
    //     digitalWrite(_rst, LOW);
    //     delay(30);  //T2>=10ms
    //     if (_interrupt > 0)
    //         digitalWrite(_interrupt, HIGH);
    //     delay(10);
    //     if (_interrupt > 0)
    //         pinMode(_interrupt, OPEN_DRAIN);
    //     digitalWrite(_rst, HIGH);
    // }

    


    softReset();

    uint8_t buffer[5] = {0};
    readBytes(GT9XX_PRODUCT_ID, buffer, 3);
    buffer[3] = readRegister(GT9XX_CONFIG_VERSION);
    //Serial.printf("TouchPad_ID:%c,%c,%c\r\nTouchPad_Config_Version:%2x\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    if(buffer[0] == '9'){
        initialization = true;
    }else{
        initialization = false;
        return false;
    }
    uint8_t check_sum = 0;
    for (int i = 0; i < (sizeof(config0) - 2); i++) {
        check_sum += config0[i];
    }
    config0[184] = (~check_sum) + 1;
    writeBytes(GT9XX_CONFIG, config0, sizeof(config0));

    return true;
}

uint8_t GT9xx_Class::readRegister(uint16_t reg)
{
    uint8_t value;
    (void)readBytes(reg, &value, 1);
    return value;
}

void GT9xx_Class::writeRegister(uint16_t reg, uint8_t value)
{
    (void)writeBytes(reg, &value, 1);
}

bool GT9xx_Class::readBytes( uint16_t reg, uint8_t *data, int nbytes)
{
    if (_readCallbackFunc != nullptr) {
        return _readCallbackFunc(_address, reg, data, nbytes);
    }
#ifdef ARDUINO
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(reg >> 8);
    _i2cPort->write(reg & 0xFF);
    _i2cPort->endTransmission();
    _i2cPort->requestFrom(_address, (uint8_t )nbytes);
    int index = 0;
    while (_i2cPort->available())
        data[index++] = _i2cPort->read();
#endif
    return nbytes == index;
}

bool GT9xx_Class::writeBytes(uint16_t reg, uint8_t *data, int nbytes)
{
    if (_writeCallbackFunc != nullptr) {
        return _writeCallbackFunc(_address, reg, data, nbytes);
    }
#ifdef ARDUINO
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(reg >> 8);
    _i2cPort->write(reg & 0xFF);
    for (int i = 0; i < nbytes; i++) {
        _i2cPort->write(data[i]);
    }
    return _i2cPort->endTransmission() != 0;
#endif
}

void GT9xx_Class::setPins(int rst, int interrupt)
{
    _rst = rst;
    _interrupt = interrupt;
}

void GT9xx_Class::softReset(void)
{
    writeRegister(GT9XX_COMMAND, 0x01);
}

bool GT9xx_Class::begin(TwoWire &port, uint8_t addr)
{
    _i2cPort = &port;
    _address = addr;
    _readCallbackFunc = nullptr;
    _writeCallbackFunc = nullptr;
    return probe();
}

bool GT9xx_Class::begin(iic_com_fptr_t read_cb, iic_com_fptr_t write_cb, uint8_t addr)
{
    if (read_cb == nullptr || write_cb == nullptr) {
        return false;
    }
    _readCallbackFunc = read_cb;
    _writeCallbackFunc = write_cb;
    _address = addr;
    return probe();
}

uint8_t GT9xx_Class::scanPoint()
{
    uint8_t point = 0;
    uint8_t buffer[40] = {0};
    if (!readBytes(GT9XX_COORDINATE, buffer, 40)) {
        //Serial.printf("Read bytes KO scanpoint:%c,%c,%c\r\nTouchPad_Config_Version:%2x\r\n", buffer[0], buffer[1], buffer[2], buffer[3]);
        return false;
    }
    writeRegister(GT9XX_CLEARBUF, 0);

    point = buffer[0] & 0xF;
    //Serial.printf("Read bytes OK point:%d\r\n", point);

    if (point == 0) {
        return 0;
    }

    data[0].x = ((uint16_t)buffer[3] << 8) + buffer[2];
    data[0].y = ((uint16_t)buffer[5] << 8) + buffer[4];

    data[1].x = ((uint16_t)buffer[11] << 8) + buffer[10];
    data[1].x = ((uint16_t)buffer[13] << 8) + buffer[12];

    data[2].x = ((uint16_t)buffer[19] << 8) + buffer[18];
    data[2].y = ((uint16_t)buffer[21] << 8) + buffer[20];

    data[3].x = ((uint16_t)buffer[27] << 8) + buffer[26];
    data[3].y = ((uint16_t)buffer[29] << 8) + buffer[28];

    data[4].x = ((uint16_t)buffer[35] << 8) + buffer[34];
    data[4].y = ((uint16_t)buffer[37] << 8) + buffer[36];

    return point;
}

void GT9xx_Class::getPoint(uint16_t &x, uint16_t &y, uint8_t index)
{
    if (index >= 4)return;
    x = data[index].x;
    y = data[index].y;
}

