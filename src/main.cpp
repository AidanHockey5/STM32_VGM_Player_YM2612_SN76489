#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "SdFat.h"
#include "U8g2lib.h"
#include "LTC6904.h"
#include "YM2612.h"
#include "SN76489.h"
#include "SPIRAM.h"
#include "logo.h"
#include "Bus.h"
#include "CircularBuffer.h"

//Prototypes
void setup();
void loop();
void tick();
void fillBuffer();
bool topUpBufffer(); 
void clearBuffers();
uint8_t readBuffer();
uint16_t readBuffer16();
uint32_t readBuffer32();
uint16_t parseVGM();

//Clocks
LTC6904 ymClock(0);
LTC6904 snClock(1);

//RAM
SPIRAM ram(PB12);

//OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

//Data Bus
Bus bus = Bus();

//Sound Chips
YM2612 ym2612(&bus);
SN76489 sn76489(&bus);

//SD
SdFat SD;
File file;

//Buffers
#define CMD_BUFFER_SIZE 10240
CircularBuffer<uint8_t, CMD_BUFFER_SIZE> cmdBuffer;
uint8_t sdBuffer[512];

//Counters
uint64_t sampleCounter = 0;
uint32_t bufferPos = 0;
uint32_t sdBlock = 0;
uint16_t waitSamples = 0;
uint32_t totalSamples = 0;

//VGM Variables
uint32_t loopOffset = 0;

void setup()
{
  //COM
  Wire.begin();
  SPI.begin();
  Serial.begin(9600);

  //Clocks
  ymClock.SetFrequency(7670453); //PAL 7600489 //NTSC 7670453
  snClock.SetFrequency(3579545); //PAL 3546894 //NTSC 3579545 

  //Chip reset
  ym2612.Reset();
  sn76489.Reset();

  //RAM
  ram.Init();
  while(!Serial){};

  //SD
  if(!SD.begin(PA4, SD_SCK_HZ(F_CPU/2)))
  {
    Serial.println("SD Mount Failed!");
  }
  if(file.isOpen())
    file.close();
  file = SD.open("36 - Because You're the Number One (Name Entry Ace Ranking)", FILE_READ);
  if(!file)
  {
    Serial.println("File open failed!");
    while(1){};
  }
  clearBuffers();
  fillBuffer();

  //Variables
  sampleCounter = 0;

  //44.1KHz tick
  Timer2.pause();
  Timer2.setPrescaleFactor(1);
  Timer2.setOverflow(1633);
  Timer2.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  Timer2.attachCompare1Interrupt(tick);
  Timer2.refresh();
  Timer2.resume();
  for(int i = 0; i<0x17; i++)
    readBuffer();
  totalSamples = readBuffer32();
  loopOffset = readBuffer32();
  for(int i = 0x20; i < 0x40; i++)
    readBuffer();
  Serial.print("LOOP: "); Serial.println(loopOffset, HEX);
}

//Count at 44.1KHz
void tick()
{
  if(waitSamples > 0)
    waitSamples--;
}

//Completely fill command buffer
void fillBuffer()
{
  while(!topUpBufffer()){};
}

//Add one 512B section from SD card to buffer. Returns true when buffer is full
bool topUpBufffer() 
{
  if(cmdBuffer.available() < 512)
    return true;
  file.read(sdBuffer, 512);
  for(int i = 0; i<512; i++)
  {
    cmdBuffer.push(sdBuffer[i]);
  }
  bufferPos = 0;
  return false;
}

void clearBuffers()
{
  bufferPos = 0;
  cmdBuffer.clear();
}

uint8_t readBuffer()
{
  if(cmdBuffer.isEmpty()) //Buffer exauhsted prematurely. Force replenish
  {
    topUpBufffer();
  }
  bufferPos++;
  return cmdBuffer.shift();
}

uint16_t readBuffer16()
{
  uint16_t d;
  for ( int i = 0; i < 2; i++ )
  {
    d += ( uint16_t( readBuffer() ) << ( 8 * i ));
  }
  bufferPos+=2;
  return d;
}

uint32_t readBuffer32()
{
  uint16_t d;
  for ( int i = 0; i < 4; i++ )
  {
    d += ( uint16_t( readBuffer() ) << ( 8 * i ));
  }
  bufferPos+=4;
  return d;
}

uint16_t parseVGM() //Execute next VGM command set. Return back wait time in samples
{
  uint8_t cmd = readBuffer();
  switch(cmd)
  {
    case 0x4F:
    sn76489.SendDataPins(0x06);
    sn76489.SendDataPins(readBuffer());
    return 1;
    case 0x50:
    sn76489.SendDataPins(readBuffer());
    return 1;
    case 0x52:
    {
    uint8_t addr = readBuffer();
    uint8_t data = readBuffer();
    ym2612.SendDataPins(addr, data, 0);
    }
    return 1;
    case 0x53:
    {
    uint8_t addr = readBuffer();
    uint8_t data = readBuffer();
    ym2612.SendDataPins(addr, data, 1);
    }
    return 1;
    case 0x61:
    return readBuffer16();
    case 0x62:
    return 735;
    case 0x63:
    return 882;

    //PCM 0x67
    case 0x67:
    {

    }
    return 0;

    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
    {
      return (cmd & 0x0F)+1;
    }
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F:
    {
      //Do PCM shit here
      uint32_t wait = cmd & 0x0F;
      uint8_t addr = 0x2A;
      return wait;
    }
    case 0xE0:
    {}
    //PCMseek
    return 0;
    case 0x66:
    {
    clearBuffers();
    file.seek((168-0x1C)-1);
    ///////////////////////////////TO DO - FIX LOOP

    topUpBufffer();

    }
    
    return 0;
    default:

    return 0;
  }
  return 0;
}

void loop()
{
  if(waitSamples == 0)
  {
    waitSamples += parseVGM();
  }
  else if(waitSamples >= 23) //It takes more or less 23 samples worth of time to read-in 512 bytes from the SD card
    {topUpBufffer();}
}