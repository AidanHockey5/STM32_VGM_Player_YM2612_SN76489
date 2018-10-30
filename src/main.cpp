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


//TEMP DELETE ON FINAL Version
#define TEST_TRACK "03 - Don't Go Off (Course Select)"
#define DEBUG_LED PA8


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
#define MAX_PCM_BUFFER_SIZE 124000

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
uint32_t cmdPos = 0;
uint32_t sdBlock = 0;
uint16_t waitSamples = 0;
uint32_t totalSamples = 0;
uint32_t pcmBufferPosition = 0;

//VGM Variables
uint32_t loopOffset = 0;
uint32_t vgmOffset = 0;
uint16_t loopCount = 0;

void setup()
{
  //DEBUG
  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, LOW);

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
  file = SD.open(TEST_TRACK, FILE_READ);
  if(!file)
  {
    Serial.println("File open failed!");
    while(1){};
  }
  clearBuffers();
  fillBuffer();

  //Variables
  sampleCounter = 0;
  cmdPos = 0;
  waitSamples = 0;

  //44.1KHz tick
  Timer2.pause();
  Timer2.setPrescaleFactor(1);
  Timer2.setOverflow(1633);
  Timer2.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  Timer2.attachCompare1Interrupt(tick);
  Timer2.refresh();
  Timer2.resume();

  //VGM Header
  readBuffer32(); //VGM
  readBuffer32(); //EoF
  readBuffer32(); //Version
  readBuffer32(); //SN Clock
  readBuffer32(); //YM2413 Clock
  readBuffer32(); //GD3 Offset
  totalSamples = readBuffer32(); //Total Samples
  loopOffset = readBuffer32(); //Loop Offset
  readBuffer32(); //Loop # Samples
  readBuffer32(); //Rate
  readBuffer32(); //SN etc.
  readBuffer32(); //YM2612 Clock
  readBuffer32(); //YM2151 Clock
  vgmOffset = readBuffer32(); //VGM data Offset
  readBuffer32(); //Sega PCM Clock
  readBuffer32(); //SPCM Interface

  //Jump to VGM data start
  if(vgmOffset == 0x00)
    vgmOffset = 0x40;
  if(vgmOffset != 0x40)
  {
    for(uint32_t i = 0x40; i<vgmOffset; i++)
      readBuffer();
  }

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
  cmdPos++;
  return cmdBuffer.shift();
}

uint16_t readBuffer16()
{
  uint16_t d;
  byte v0 = readBuffer();
  byte v1 = readBuffer();
  d = uint16_t(v0 + (v1 << 8));
  bufferPos+=2;
  cmdPos+=2;
  return d;
}

uint32_t readBuffer32()
{
  uint16_t d;
  byte v0 = readBuffer();
  byte v1 = readBuffer();
  byte v2 = readBuffer();
  byte v3 = readBuffer();
  d = uint32_t(v0 + (v1 << 8) + (v2 << 16) + (v3 << 24));
  bufferPos+=4;
  cmdPos+=4;
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
    case 0x67:
    {
      readBuffer16(); //Discard 0x66 and datatype byte
      pcmBufferPosition = cmdPos;
      uint32_t PCMSize = readBuffer32();
      if(PCMSize > MAX_PCM_BUFFER_SIZE)
      {
        while(1)
        {
          Serial.println("PCM Size too big!");
          delay(500);
        }
      }
      for (uint32_t i = 0; i < PCMSize; i++)
      {
        ram.WriteByte(i, readBuffer());
      }
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
      //Set RAM to read sequentially? Manual SPI clock pulsing will be required. Rewrite RAM driver.
      uint32_t wait = cmd & 0x0F;
      uint8_t addr = 0x2A;
      uint8_t data = ram.ReadByte(pcmBufferPosition++);
      ym2612.SendDataPins(addr, data, 0);
      return wait;
    }
    case 0xE0:
    {
      pcmBufferPosition = readBuffer32();
    }
    //PCMseek
    return 0;
    case 0x66:
    {
    clearBuffers();
    cmdPos = 0;
    loopOffset == 0x40 ? file.seek(0x40) : file.seek(loopOffset-0x1C);
    topUpBufffer();
    loopCount++;
    }
    
    return 0;
    default:

    return 0;
  }
  return 0;
}

bool debugOn = false;
void loop()
{
  if(cmdBuffer.isEmpty())
  {
    debugOn = true;
    digitalWrite(DEBUG_LED, HIGH);
  }
  else if(debugOn)
  {
    debugOn = false;
    digitalWrite(DEBUG_LED, LOW);
  }
    
  if(waitSamples == 0)
  {
    waitSamples += parseVGM();
  }
  else if(waitSamples >= 23) //It takes more or less 23 samples worth of time to read-in 512 bytes from the SD card
    {topUpBufffer();}
}