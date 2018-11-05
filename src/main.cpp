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
#include "TrackStructs.h"
#include "ringbuffer.h"

//Debug variables
#define DEBUG false //Set this to true for a detailed printout of the header data & any errored commnand bytes
#define DEBUG_LED PA8
bool commandFailed = false;
uint8_t failedCmd = 0x00;

//Prototypes
void setup();
void loop();
void handleSerialIn();
void tick();
void removeSVI();
void prebufferLoop();
void injectPrebuffer();
void fillBuffer();
bool topUpBufffer(); 
void clearBuffers();
void handleButtons();
void prepareChips();
void readGD3();
void setISR();
void drawOLEDTrackInfo();
bool startTrack(FileStrategy fileStrategy, String request = "");
bool vgmVerify();
uint8_t readBuffer();
uint16_t readBuffer16();
uint32_t readBuffer32();
uint32_t readSD32();
uint16_t parseVGM();

//Clocks
LTC6904 ymClock(0);
LTC6904 snClock(1);

//RAM
SPIRAM ram(PB12);
#define MAX_PCM_BUFFER_SIZE 124000
uint8_t ramPrefetch = 0x00;
bool ramPrefetchFlag = false;

//OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0);
bool isOledOn = true;

//Data Bus
Bus bus = Bus();

//Buttons
const int prev_btn = PB11;
const int rand_btn = PB10;
const int next_btn = PB1;
const int option_btn = PA3;

//Sound Chips
YM2612 ym2612(&bus);
SN76489 sn76489(&bus);

//SD & File Streaming
SdFat SD;
File file;
#define MAX_FILE_NAME_SIZE 128
char fileName[MAX_FILE_NAME_SIZE];
uint32_t numberOfFiles = 0;
uint32_t currentFileNumber = 0;

//Buffers
#define CMD_BUFFER_SIZE 8192
#define LOOP_PREBUF_SIZE 512
typedef ringbuffer_t<uint8_t, CMD_BUFFER_SIZE, uint8_t> RingBuffer;
static RingBuffer cmdBuffer;
uint8_t loopPreBuffer[LOOP_PREBUF_SIZE];

//Counters
uint32_t bufferPos = 0;
uint32_t cmdPos = 0;
uint16_t waitSamples = 0;
uint32_t pcmBufferPosition = 0;

//VGM Variables
uint16_t loopCount = 0;
uint8_t maxLoops = 3;
bool fetching = false;
volatile bool ready = false;
bool samplePlaying = false;
VGMHeader header;
GD3 gd3;
PlayMode playMode = SHUFFLE;

void setup()
{
  //DEBUG
  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, LOW);

  //Buttons
  pinMode(prev_btn, INPUT_PULLUP);
  pinMode(rand_btn, INPUT_PULLUP);
  pinMode(next_btn, INPUT_PULLUP);
  pinMode(option_btn, INPUT_PULLUP);

  //COM
  Wire.begin();
  SPI.begin();
  Serial.begin(115200);

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
    oled.clearBuffer();
    oled.drawStr(0,16,"SD Mount");
    oled.drawStr(0,32,"failed!");
    oled.sendBuffer();
    while(true){}
  }

  //OLED
  oled.begin();
  oled.setFont(u8g2_font_fub11_tf);
  oled.drawXBM(0,0, logo_width, logo_height, logo);
  oled.sendBuffer();
  delay(3000);
  oled.clearDisplay();

  //Prepare files
  removeSVI();

  File countFile;
  while ( countFile.openNext( SD.vwd(), O_READ ))
  {
    countFile.close();
    numberOfFiles++;
  }
  countFile.close();
  SD.vwd()->rewind();

  //44.1KHz tick
  setISR();

  //Begin
  startTrack(FIRST_START);
  vgmVerify();
  prepareChips();
}

void setISR()
{
  Timer2.pause();
  Timer2.setPrescaleFactor(1);
  Timer2.setOverflow(1633);
  Timer2.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  Timer2.attachCompare1Interrupt(tick);
  Timer2.refresh();
  Timer2.resume();  
}

void drawOLEDTrackInfo()
{
  ready = false;
  if(isOledOn)
  {
    oled.setPowerSave(0);
    oled.clearDisplay();
    oled.setFont(u8g2_font_helvR08_te);
    oled.sendBuffer();
    char *cstr = &gd3.enTrackName[0u];
    oled.drawStr(0,10, cstr);
    cstr = &gd3.enGameName[0u];
    oled.drawStr(0,20, cstr);
    cstr = &gd3.releaseDate[0u];
    oled.drawStr(0,30, cstr);
    cstr = &gd3.enSystemName[0u];
    oled.drawStr(0,40, cstr);
    String fileNumberData = "File: " + String(currentFileNumber+1) + "/" + String(numberOfFiles);
    cstr = &fileNumberData[0u];
    oled.drawStr(0,50, cstr);
    String playmodeStatus;
    if(playMode == LOOP)
      playmodeStatus = "LOOP";
    else if(playMode == SHUFFLE)
      playmodeStatus = "SHUFFLE";
    else
      playmodeStatus = "IN ORDER";
    cstr = &playmodeStatus[0u];
    oled.drawStr(0, 60, cstr);
    oled.sendBuffer();
  }
  else
  {
    oled.clearDisplay();
    oled.setPowerSave(1);
    oled.sendBuffer();
  }
  ready = true;
}

void prepareChips()
{
  //Clocks
  ymClock.SetFrequency(header.ym2612Clock); //PAL 7600489 //NTSC 7670453
  snClock.SetFrequency(header.sn76489Clock); //PAL 3546894 //NTSC 3579545 

  //Chip reset
  ym2612.Reset();
  sn76489.Reset();
}

//Mount file and prepare for playback. Returns true if file is found.
bool startTrack(FileStrategy fileStrategy, String request)
{
  ready = false;
  File nextFile;
  memset(fileName, 0x00, MAX_FILE_NAME_SIZE);

  switch(fileStrategy)
  {
    case FIRST_START:
    {
      nextFile.openNext(SD.vwd(), O_READ);
      nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
      nextFile.close();
      currentFileNumber = 0;
    }
    break;
    case NEXT:
    {
      if(currentFileNumber+1 >= numberOfFiles)
      {
          SD.vwd()->rewind();
          currentFileNumber = 0;
      }
      else
          currentFileNumber++;
      nextFile.openNext(SD.vwd(), O_READ);
      nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
      nextFile.close();
    }
    break;
    case PREV:
    {
      if(currentFileNumber != 0)
      {
        currentFileNumber--;
        SD.vwd()->rewind();
        for(uint32_t i = 0; i<=currentFileNumber; i++)
        {
          nextFile.close();
          nextFile.openNext(SD.vwd(), O_READ);
        }
        nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
        nextFile.close();
      }
      else
      {
        currentFileNumber = numberOfFiles-1;
        SD.vwd()->rewind();
        for(uint32_t i = 0; i<=currentFileNumber; i++)
        {
          nextFile.close();
          nextFile.openNext(SD.vwd(), O_READ);
        }
        nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
        nextFile.close();
      }
    }
    break;
    case RND:
    {
      randomSeed(micros());
      uint32_t randomFile = currentFileNumber;
      if(numberOfFiles > 1)
      {
        while(randomFile == currentFileNumber)
          randomFile = random(numberOfFiles-1);
      }
      currentFileNumber = randomFile;
      SD.vwd()->rewind();
      nextFile.openNext(SD.vwd(), O_READ);
      {
        for(uint32_t i = 0; i<randomFile; i++)
        {
          nextFile.close();
          nextFile.openNext(SD.vwd(), O_READ);
        }
      }
      nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
      nextFile.close();
    }
    break;
    case REQUEST:
    {
      SD.vwd()->rewind();
      bool fileFound = false;
      Serial.print("REQUEST: ");Serial.println(request);
      for(uint32_t i = 0; i<numberOfFiles; i++)
      {
        nextFile.close();
        nextFile.openNext(SD.vwd(), O_READ);
        nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
        String tmpFN = String(fileName);
        tmpFN.trim();
        request.trim();
        if(tmpFN == request)
        {
          currentFileNumber = i;
          fileFound = true;
          break;
        }
      }
      nextFile.close();
      if(fileFound)
      {
        Serial.println("File found!");
      }
      else
      {
        Serial.println("ERROR: File not found! Continuing with current song.");
        ready = true;
        return false;
      }
    }
    break;
  }

  cmdPos = 0;
  bufferPos = 0;
  waitSamples = 0;
  loopCount = 0;

  if(file.isOpen())
    file.close();
  file = SD.open(fileName, FILE_READ);
  if(!file)
    Serial.println("Failed to read file");

  clearBuffers();
  memset(&loopPreBuffer, 0, LOOP_PREBUF_SIZE);
  header.Reset();
  fillBuffer();

  //VGM Header
  header.indent = readBuffer32(); //VGM
  header.EoF = readBuffer32(); //EoF
  header.version = readBuffer32(); //Version
  header.sn76489Clock = readBuffer32(); //SN Clock
  header.ym2413Clock = readBuffer32(); //YM2413 Clock
  header.gd3Offset = readBuffer32(); //GD3 Offset
  header.totalSamples = readBuffer32(); //Total Samples
  header.loopOffset = readBuffer32(); //Loop Offset
  header.loopNumSamples = readBuffer32(); //Loop # Samples
  header.rate = readBuffer32(); //Rate
  header.snX = readBuffer32(); //SN etc.
  header.ym2612Clock = readBuffer32(); //YM2612 Clock
  header.ym2151Clock = readBuffer32(); //YM2151 Clock
  header.vgmDataOffset = readBuffer32(); //VGM data Offset
  header.segaPCMClock = readBuffer32(); //Sega PCM Clock
  header.spcmInterface = readBuffer32(); //SPCM Interface

  #if DEBUG
  Serial.print("Indent: 0x"); Serial.println(header.indent, HEX);
  Serial.print("EoF: 0x"); Serial.println(header.EoF, HEX);
  Serial.print("Version: 0x"); Serial.println(header.version, HEX);
  Serial.print("SN Clock: "); Serial.println(header.sn76489Clock);
  Serial.print("YM2413 Clock: "); Serial.println(header.ym2413Clock);
  Serial.print("GD3 Offset: 0x"); Serial.println(header.gd3Offset, HEX);
  Serial.print("Total Samples: "); Serial.println(header.totalSamples);
  Serial.print("Loop Offset: 0x"); Serial.println(header.loopOffset, HEX);
  Serial.print("Loop # Samples: "); Serial.println(header.loopNumSamples);
  Serial.print("Rate: "); Serial.println(header.rate);
  Serial.print("SN etc.: 0x"); Serial.println(header.snX, HEX);
  Serial.print("YM2612 Clock: "); Serial.println(header.ym2612Clock);
  Serial.print("YM2151 Clock: "); Serial.println(header.ym2151Clock);
  Serial.print("VGM data Offset: 0x"); Serial.println(header.vgmDataOffset, HEX);
  Serial.print("SPCM Interface: 0x"); Serial.println(header.spcmInterface, HEX);
  #endif

  //Jump to VGM data start and compute loop location
  if(header.vgmDataOffset == 0x0C)
    header.vgmDataOffset = 0x40;
  else
    header.vgmDataOffset += 0x34;
  
  if(header.vgmDataOffset != 0x40)
  {
    for(uint32_t i = 0x40; i<header.vgmDataOffset; i++)
      readBuffer();
  }
  if(header.loopOffset == 0x00)
  {
    header.loopOffset = header.vgmDataOffset;
  }
  else
    header.loopOffset += 0x1C;

  prebufferLoop();
  #if DEBUG
  //Dump the contents of the prebuffer
  for(int i = 0; i<LOOP_PREBUF_SIZE; i++)
  {
    if(i % 32 == 0)
      Serial.println();
    Serial.print("0x"); Serial.print(loopPreBuffer[i], HEX); Serial.print(", ");
  }
  #endif
  ramPrefetch = ram.ReadByte(pcmBufferPosition++);
  return true;
}

bool vgmVerify()
{
  if(header.indent != 0x206D6756) //VGM. Indent check
  {
    startTrack(NEXT);
    return false;
  }
  Serial.println("VGM OK!");
  readGD3();
  drawOLEDTrackInfo();
  Serial.println(gd3.enGameName);
  Serial.println(gd3.enTrackName);
  Serial.println(gd3.enSystemName);
  Serial.println(gd3.releaseDate);
  Serial.print("Version: "); Serial.println(header.version, HEX);
  ready = true;
  return true;
}

void readGD3()
{
  uint32_t prevLocation = file.curPosition();
  uint32_t tag = 0;
  gd3.Reset();
  file.seek(0);
  file.seek(header.gd3Offset+0x14);
  for(int i = 0; i<4; i++) {tag += uint32_t(file.read());} //Get GD3 tag bytes and add them up for an easy comparison.
  if(tag != 0xFE) //GD3 tag bytes do not sum up to the constant. No valid GD3 data detected. 
  {Serial.print("INVALID GD3 SUM:"); Serial.println(tag); file.seekSet(prevLocation); return;}
  for(int i = 0; i<4; i++) {file.read();} //Skip version info
  uint8_t v[4];
  file.readBytes(v,4);
  gd3.size = uint32_t(v[0] + (v[1] << 8) + (v[2] << 16) + (v[3] << 24));
  char a, b;
  uint8_t itemIndex = 0;
  for(uint32_t i = 0; i<gd3.size; i++)
  {
    a = file.read();
    b = file.read();
    if(a+b == 0) //Double 0 detected
    {
      itemIndex++;
      continue;
    }
    switch(itemIndex)
    {
      case 0:
      gd3.enTrackName += a;
      break;
      case 1:
      //JP TRACK NAME
      break;
      case 2:
      gd3.enGameName += a;
      break;
      case 3:
      //JP GAME NAME
      break;
      case 4:
      gd3.enSystemName += a;
      break;
      case 5:
      //JP SYSTEM NAME
      break;
      case 6:
      gd3.enAuthor += a;
      break;
      case 7:
      //JP AUTHOR
      break;
      case 8:
      gd3.releaseDate += a;
      break;
      default:
      //IGNORE CONVERTER NAME + NOTES
      break;
    }
  }
  file.seekSet(prevLocation);
}

void removeSVI() //Sometimes, Windows likes to place invisible files in our SD card without asking... GTFO!
{
  File nextFile;
  nextFile.openNext(SD.vwd(), O_READ);
  char name[MAX_FILE_NAME_SIZE];
  nextFile.getName(name, MAX_FILE_NAME_SIZE);
  String n = String(name);
  if(n == "System Volume Information")
  {
      if(!nextFile.rmRfStar())
        Serial.println("Failed to remove SVI file");
  }
  SD.vwd()->rewind();
  nextFile.close();
}

//Keep a small cache of commands right at the loop point to prevent excessive SD seeking lag
void prebufferLoop() 
{
  uint32_t prevPos = file.curPosition();
  file.seekSet(header.loopOffset);
  file.readBytes(loopPreBuffer, LOOP_PREBUF_SIZE);
  file.seekSet(prevPos);
  #if DEBUG
  Serial.print("FIRST LOOP BYTE: "); Serial.println(loopPreBuffer[0], HEX);
  #endif
}

//On loop, inject the small prebuffer back into the main ring buffer
void injectPrebuffer()
{
  for(int i = 0; i<LOOP_PREBUF_SIZE; i++)
    cmdBuffer.push_back(loopPreBuffer[i]);
  file.seekSet(header.loopOffset+LOOP_PREBUF_SIZE);
  cmdPos = LOOP_PREBUF_SIZE-1;
  #if DEBUG
  Serial.println(file.curPosition());
  #endif
}

//Completely fill command buffer
void fillBuffer()
{
  while(!topUpBufffer()){};
}

//Add to buffer from SD card. Returns true when buffer is full
bool topUpBufffer() 
{
  if(cmdBuffer.full())
    return true;
  if(cmdBuffer.available() >= file.size()) 
     return true;
  fetching = true;
  cmdBuffer.push_back_nc(file.read());
  bufferPos = 0;
  fetching = false;
  return false;
}

void clearBuffers()
{
  bufferPos = 0;
  cmdBuffer.clear();
}

uint8_t readBuffer()
{
  if(cmdBuffer.empty()) //Buffer exauhsted prematurely. Force replenish
  {
    topUpBufffer();
  }
  bufferPos++;
  cmdPos++;
  return cmdBuffer.pop_front_nc();
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
  uint32_t d;
  byte v0 = readBuffer();
  byte v1 = readBuffer();
  byte v2 = readBuffer();
  byte v3 = readBuffer();
  d = uint32_t(v0 + (v1 << 8) + (v2 << 16) + (v3 << 24));
  bufferPos+=4;
  cmdPos+=4;
  return d;
}

//Read 32 bits right off of the SD card.
uint32_t readSD32()
{
  uint32_t d;
  byte v[4];
  file.readBytes(v, 4);
  d = uint32_t(v[0] + (v[1] << 8) + (v[2] << 16) + (v[3] << 24));
  return d;
}

//Count at 44.1KHz
void tick()
{
  if(!ready || cmdBuffer.empty())
    return;
  if(waitSamples > 0)
    waitSamples--;
  if(waitSamples == 0 && !samplePlaying)
  {
    samplePlaying = true;
    waitSamples += parseVGM();
    samplePlaying = false;
    return;
  }
}

//Execute next VGM command set. Return back wait time in samples
uint16_t parseVGM() 
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
        startTrack(NEXT);
        Serial.println("PCM Size too big!");
      }
      for (uint32_t i = 0; i < PCMSize; i++)
      {
        ram.WriteByte(i, readBuffer());
      }
      fillBuffer();
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
      //RAM Prefetching. Store the next byte of PCM sample data in a char that will cache itself between samples
      ramPrefetchFlag = true;
      uint32_t wait = cmd & 0x0F;
      uint8_t addr = 0x2A;
      uint8_t data = ramPrefetch;
      pcmBufferPosition++;
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
    ready = false;
    clearBuffers();
    cmdPos = 0;
    injectPrebuffer();
    loopCount++;
    ready = true;
    }
    return 0;
    default:
    commandFailed = true;
    failedCmd = cmd;
    return 0;
  }
  return 0;
}

//Poll the serial port
void handleSerialIn()
{
  bool newTrack = false;
  while(Serial.available())
  {
    char serialCmd = Serial.read();
    switch(serialCmd)
    {
      case '+':
        newTrack = startTrack(NEXT);
      break;
      case '-':
        newTrack = startTrack(PREV);
      break;
      case '*':
        newTrack = startTrack(RND);
      break;
      case '/':
        playMode = SHUFFLE;
        drawOLEDTrackInfo();
      break;
      case '.':
        playMode = LOOP;
        drawOLEDTrackInfo();
      break;
      case '?':
        Serial.println(gd3.enGameName);
        Serial.println(gd3.enTrackName);
        Serial.println(gd3.enSystemName);
        Serial.println(gd3.releaseDate);
        Serial.print("Version: "); Serial.println(header.version, HEX);
      break;
      case '!':
        isOledOn = !isOledOn;
        drawOLEDTrackInfo();
      break;
      case 'r':
      {
        String req = Serial.readString();
        req.remove(0, 1); //Remove colon character
        newTrack = startTrack(REQUEST, req);
      }
      break;
      default:
        continue;
    }
  }
  if(newTrack)
  {
    vgmVerify();
    prepareChips();
  }
}

//Check for button input
bool buttonLock = false;
void handleButtons()
{
  bool newTrack = false;
  bool togglePlaymode = false;
  uint32_t count = 0;
  
  if(!digitalRead(next_btn))
    newTrack = startTrack(NEXT);
  if(!digitalRead(prev_btn))
    newTrack = startTrack(PREV);
  if(!digitalRead(rand_btn))
    newTrack = startTrack(RND);
  if(!digitalRead(option_btn))
    togglePlaymode = true;
  else
    buttonLock = false;
  while(!digitalRead(option_btn))
  {
    if(count >= 100) 
    {
      //toggle OLED after one second of holding OPTION button
      isOledOn = !isOledOn;
      drawOLEDTrackInfo();
      togglePlaymode = false;
      buttonLock = true;
      break;
    } 
    delay(10);
    count++;
  }
  if(buttonLock)
    togglePlaymode = false;
  if(newTrack)
  {
    vgmVerify();
    prepareChips();
    delay(100);
  }
  if(togglePlaymode)
  {
    togglePlaymode = false;
    if(playMode == SHUFFLE)
      playMode = LOOP;
    else if(playMode == LOOP)
      playMode = IN_ORDER;
    else if(playMode == IN_ORDER)
      playMode = SHUFFLE;
    drawOLEDTrackInfo();
  }
}

void loop()
{    
  topUpBufffer();
  if(ramPrefetchFlag)
  {
    ramPrefetch = ram.ReadByte(pcmBufferPosition);
    ramPrefetchFlag = false;
  }
  if(loopCount >= maxLoops && playMode != LOOP)
  {
    bool newTrack = false;
    if(playMode == SHUFFLE)
      newTrack = startTrack(RND);
    if(playMode == IN_ORDER)
      newTrack = startTrack(NEXT);
    if(newTrack)
    {
      vgmVerify();
      prepareChips();
    }
  }
  if(Serial.available() > 0)
    handleSerialIn();
  handleButtons();
  #if DEBUG
  if(commandFailed)
  {
    commandFailed = false;
    Serial.print("CMD ERROR: "); Serial.println(failedCmd, HEX);
  }
  #endif
}