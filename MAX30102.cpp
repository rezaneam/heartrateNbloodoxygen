/*
*  MAX30102 driver - uses a combination of ideas from the Maxim & Sparkfun drivers
*                    used Technolbogy's Wire 
*
* j.n.magee 15-10-2019
*/

#include <Wire.h>
#include "MAX30102.h"

static const uint8_t MAX_30102_ID = 0x15;

MAX30102::MAX30102()
{
  // Constructor
}

boolean MAX30102::begin(TwoWire &i2c, uint8_t i2caddr)
{
  i2cBus = &i2c;
  _i2caddr = i2caddr;
  if (readRegister8(REG_PART_ID) != MAX_30102_ID)
  {
    printf("read value %X\r\n", readRegister8(REG_PART_ID));
    return false;
  }
  return true;
}

void MAX30102::setup()
{
  writeRegister8(REG_MODE_CONFIG, 0x40); //reset
  delay(500);
  writeRegister8(REG_FIFO_WR_PTR, 0x00); //FIFO_WR_PTR[4:0]
  writeRegister8(REG_OVF_COUNTER, 0x00); //OVF_COUNTER[4:0]
  writeRegister8(REG_FIFO_RD_PTR, 0x00); //FIFO_RD_PTR[4:0]
  writeRegister8(REG_FIFO_CONFIG, 0x4f); //sample avg = 4, fifo rollover=false, fifo almost full = 17
  writeRegister8(REG_MODE_CONFIG, 0x03); //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
  writeRegister8(REG_SPO2_CONFIG, 0x27); // SPO2_ADC=4096nA, SPO2 sample rate(100Hz), pulseWidth (411uS)
  writeRegister8(REG_LED1_PA, 0x17);     //Choose value for ~ 6mA for LED1 (IR)
  writeRegister8(REG_LED2_PA, 0x17);     // Choose value for ~ 6mA for LED2 (Red)
  writeRegister8(REG_PILOT_PA, 0x1F);    // Choose value for ~ 6mA for Pilot LED
}

//Tell caller how many samples are available
uint8_t MAX30102::available(void)
{
  int8_t numberOfSamples = sense.head - sense.tail;
  if (numberOfSamples < 0)
    numberOfSamples += STORAGE_SIZE;
  return (numberOfSamples);
}

//Report the next Red value in the FIFO
uint32_t MAX30102::getRed(void)
{
  return (sense.red[sense.tail]);
}

//Report the next IR value in the FIFO
uint32_t MAX30102::getIR(void)
{
  return (sense.IR[sense.tail]);
}

//Advance the tail
void MAX30102::nextSample(void)
{
  if (available())
  {
    sense.tail++;
    sense.tail %= STORAGE_SIZE; //Wrap condition
  }
}

// check sensor for new samples and upload if available
uint16_t MAX30102::check(void)
{
  byte readPointer = readRegister8(REG_FIFO_RD_PTR);
  byte writePointer = readRegister8(REG_FIFO_WR_PTR);
  int numberOfSamples = 0;
  if (readPointer != writePointer)
  {
    //Calculate the number of readings we need to get from sensor
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0)
      numberOfSamples += 32;                   //Wrap condition
    int bytesLeftToRead = numberOfSamples * 6; //3 bytes each for Red and IR
    i2cBus->beginTransmission(_i2caddr);
    i2cBus->write(REG_FIFO_DATA);
    i2cBus->endTransmission();
    bytesLeftToRead = bytesLeftToRead <= 32 ? bytesLeftToRead : 32;
    i2cBus->requestFrom(_i2caddr, bytesLeftToRead);
    while (bytesLeftToRead > 0)
    {
      sense.head++;               //Advance the head of the storage struct
      sense.head %= STORAGE_SIZE; //Wrap condition
      sense.IR[sense.head] = readFIFOSample();
      //Burst read three more bytes - IR
      sense.red[sense.head] = readFIFOSample();
      bytesLeftToRead -= 6;
    }
    i2cBus->endTransmission();
  }
  return (numberOfSamples);
}

//
// Low-level I2C Communication
//
uint8_t MAX30102::readRegister8(uint8_t reg)
{
  uint8_t value;
  i2cBus->beginTransmission(_i2caddr);
  i2cBus->write((uint8_t)reg);
  i2cBus->endTransmission();
  i2cBus->requestFrom(_i2caddr, (byte)1);
  value = i2cBus->read();
  i2cBus->endTransmission();
  return value;
}

uint32_t MAX30102::readFIFOSample()
{
  byte temp[4];
  uint32_t temp32;
  temp[3] = 0;
  temp[2] = i2cBus->read();
  temp[1] = i2cBus->read();
  temp[0] = i2cBus->read();
  memcpy(&temp32, temp, 4);
  return temp32 & 0x3FFFF;
}

void MAX30102::writeRegister8(uint8_t reg, uint8_t value)
{
  i2cBus->beginTransmission(_i2caddr);
  i2cBus->write(reg);
  i2cBus->write(value);
  i2cBus->endTransmission();
}
