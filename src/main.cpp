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

LTC6904 ymClock(0);
LTC6904 snClock(1);

SPIRAM ram(PB12);

//OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
bool isOLEDOn = true;

SdFat SD;
File vgm;

const int prev_btn = PB11;
const int rand_btn = PB10;
const int next_btn = PB1;
const int option_btn = PA3;

int dataBusPins[8] = {PB8, PB9, PC13, PC14, PC15, PA0, PA1, PA2};
const int YM_CS = PB3;
const int YM_RD = PA15;
const int YM_WR = PA12;
const int YM_A0 = PA11;
const int YM_A1 = PB0;
const int YM_IC = PB4; 
const int YM_IRQ = NULL;

const int SN_WE = PB5;

YM2612 ym2612(dataBusPins, YM_CS, YM_RD, YM_WR, YM_A0, YM_A1, YM_IRQ, YM_IC);
SN76489 sn76489(dataBusPins, SN_WE);
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

//Buffer & file stream
const unsigned int MAX_CMD_BUFFER = 1;
unsigned char cmdBuffer[MAX_CMD_BUFFER];
uint32_t bufferPos = 0;
const unsigned int MAX_FILE_NAME_SIZE = 1024;
char fileName[MAX_FILE_NAME_SIZE];
unsigned char cmd = 0;
uint16_t numberOfFiles = 0;
int32_t currentFileNumber = 0;

//Timing Variables
float singleSampleWait = 0;
const int sampleRate = 44100; //44100 standard
const float WAIT60TH = ((1000.0 / (sampleRate/(float)735))*1000);
const float WAIT50TH = ((1000.0 / (sampleRate/(float)882))*1000);
uint32_t waitSamples = 0;
unsigned long preCalced8nDelays[16];
unsigned long preCalced7nDelays[16];
unsigned long lastWaitData61 = 0;
unsigned long cachedWaitTime61 = 0;
unsigned long pauseTime = 0;
unsigned long startTime = 0;

//Song Data Variables
#define MAX_PCM_BUFFER_SIZE 124000 //In bytes (Size of SPI_RAM)
//uint8_t pcmBuffer[MAX_PCM_BUFFER_SIZE];
uint32_t pcmBufferPosition = 0;
uint32_t loopOffset = 0;
uint16_t loopCount = 0;
uint16_t nextSongAfterXLoops = 3;
uint32_t rate = 60;
enum PlayMode {LOOP, PAUSE, SHUFFLE, IN_ORDER};
PlayMode playMode = SHUFFLE;

//GD3 Data
String trackTitle;
String gameName;
String systemName;
String gameDate;

// void FillBuffer()
// {
//     //vgm.readBytes(cmdBuffer, MAX_CMD_BUFFER);
//     //Serial.print("File location: "); Serial.println(vgm.position(), HEX);
// }

// unsigned char vgm.read()
// {
//   
//   // if(bufferPos == MAX_CMD_BUFFER)
//   // {
//   //   bufferPos = 0;
//   //   FillBuffer();
//   // }
//   // return cmdBuffer[bufferPos++];
// }

uint32_t Read32() //Read 32 bit value straight from SD card
{
  byte v0 = vgm.read();
  byte v1 = vgm.read();
  byte v2 = vgm.read();
  byte v3 = vgm.read();
  return uint32_t(v0 + (v1 << 8) + (v2 << 16) + (v3 << 24));
}

void ClearBuffers()
{
  bufferPos = 0;
  for(int i = 0; i < MAX_CMD_BUFFER; i++)
    cmdBuffer[i] = 0;
}

void RemoveSVI() //Sometimes, Windows likes to place invisible files in our SD card without asking... GTFO!
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


void DrawOledInfo()
{
  if(isOLEDOn)
  {
    u8g2.setPowerSave(0);
    u8g2.clearDisplay();
    u8g2.setFont(u8g2_font_helvR08_te);
    u8g2.sendBuffer();
    char *cstr = &trackTitle[0u];
    u8g2.drawStr(0,10, cstr);
    cstr = &gameName[0u];
    u8g2.drawStr(0,20, cstr);
    cstr = &gameDate[0u];
    u8g2.drawStr(0,30, cstr);
    cstr = &systemName[0u];
    u8g2.drawStr(0,40, cstr);
    String fileNumberData = "File: " + String(currentFileNumber+1) + "/" + String(numberOfFiles);
    cstr = &fileNumberData[0u];
    u8g2.drawStr(0,50, cstr);
    String loopShuffleStatus;
    String loopStatus;
    if(playMode == LOOP)
      loopStatus = "LOOP ON";
    else
      loopStatus = "LOOP OFF";
    String shuffleStatus;
    if(playMode == SHUFFLE)
      shuffleStatus = "SHFL ON";
    else
      shuffleStatus = "SHFL OFF";
    loopShuffleStatus = loopStatus + " | " + shuffleStatus;
    cstr = &loopShuffleStatus[0u];
    u8g2.drawStr(0, 60, cstr);
    u8g2.sendBuffer();
  }
  else
  {
      u8g2.clearDisplay();
      u8g2.setPowerSave(1);
      u8g2.sendBuffer();
  }
}

void ClearTrackData()
{
  for(int i = 0; i < MAX_FILE_NAME_SIZE; i++)
    fileName[i] = 0;
  trackTitle = "";
  gameName = "";
  systemName = "";
  gameDate = "";
}

uint32_t EoFOffset = 0;
uint32_t VGMVersion = 0;
uint32_t GD3Offset = 0;
void GetHeaderData() //Scrape off the important VGM data from the header, then drop down to the GD3 area for song info data
{
  Read32(); //V - G - M 0x00->0x03
  EoFOffset = Read32(); //End of File offset 0x04->0x07
  VGMVersion = Read32(); //VGM Version 0x08->0x0B
  for(int i = 0x0C; i<0x14; i++)vgm.read(); //Skip 0x0C->0x14
  GD3Offset = Read32(); //GD3 (song info) data offset 0x14->0x17

  uint32_t bufferReturnPosition = vgm.position();
  vgm.seek(0);
  vgm.seekCur(GD3Offset+0x14);
  uint32_t GD3Position = 0x00;
  Read32(); GD3Position+=4;  //G - D - 3
  Read32(); GD3Position+=4;  //Version data
  uint32_t dataLength = Read32(); //Get size of data payload
  GD3Position+=4;

  String rawGD3String;
  // Serial.print("DATA LENGTH: ");
  // Serial.println(dataLength);

  for(int i = 0; i<dataLength; i++) //Convert 16-bit characters to 8 bit chars. This may cause issues with non ASCII characters. (IE Japanese chars.)
  {
    char c1 = vgm.read();
    char c2 = vgm.read();
    if(c1 == 0 && c2 == 0)
      rawGD3String += '\n';
    else
      rawGD3String += char(c1);
  }
  GD3Position = 0;

  while(rawGD3String[GD3Position] != '\n') //Parse out the track title.
  {
    trackTitle += rawGD3String[GD3Position];
    GD3Position++;
  }
  GD3Position++;

  while(rawGD3String[GD3Position] != '\n') GD3Position++; //Skip Japanese track title.
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') //Parse out the game name.
  {
    gameName += rawGD3String[GD3Position];
    GD3Position++;
  }
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') GD3Position++;//Skip Japanese game name.
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') //Parse out the system name.
  {
    systemName += rawGD3String[GD3Position];
    GD3Position++;
  }
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') GD3Position++;//Skip Japanese system name.
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') GD3Position++;//Skip English authors
  GD3Position++;
  // while(rawGD3String[GD3Position] != 0) //Parse out the music authors (I skipped this since it sometimes produces a ton of data! Uncomment this, comment skip, add vars if you want this.)
  // {
  //   musicAuthors += rawGD3String[GD3Position];
  //   GD3Position++;
  // }
  while(rawGD3String[GD3Position] != '\n') GD3Position++;//Skip Japanese authors.
  GD3Position++;
  while(rawGD3String[GD3Position] != '\n') //Parse out the game date
  {
    gameDate += rawGD3String[GD3Position];
    GD3Position++;
  }
  GD3Position++;
  vgm.seek(bufferReturnPosition); //Send the file seek back to the original buffer position so we don't confuse our program.
  waitSamples = Read32(); //0x18->0x1B : Get wait Samples count
  loopOffset = Read32();  //0x1C->0x1F : Get loop offset Postition
  Read32();               //0x20->0x23 : Skip loop samples
  rate = Read32();        //0x24->0x27 : Get recording rate in Hz
  for(int i = 0; i<3; i++) Read32(); //Skip right to the VGM data offset position;
  uint32_t vgmDataOffset = Read32();
  if(vgmDataOffset == 0 || vgmDataOffset == 12) //VGM starts at standard 0x40
  {
    Read32(); Read32();
  }
  else
  {
    for(int i = 0; i < vgmDataOffset; i++) vgm.read();  //VGM starts at different data position (Probably VGM spec 1.7+)
  }
  Serial.println(trackTitle);
  Serial.println(gameName);
  Serial.println(systemName);
  Serial.println(gameDate);
  Serial.print("Rate: "); Serial.print(rate); Serial.println("Hz");
  Serial.println("");
  DrawOledInfo();
}

enum StartUpProfile {FIRST_START, NEXT, PREVIOUS, RNG, REQUEST};
void StartupSequence(StartUpProfile sup, String request = "")
{
  File nextFile;
  ClearTrackData();
  switch(sup)
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
    case PREVIOUS:
    {
      if(currentFileNumber != 0)
      {
        currentFileNumber--;
        SD.vwd()->rewind();
        for(int i = 0; i<=currentFileNumber; i++)
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
        for(int i = 0; i<=currentFileNumber; i++)
        {
          nextFile.close();
          nextFile.openNext(SD.vwd(), O_READ);
        }
        nextFile.getName(fileName, MAX_FILE_NAME_SIZE);
        nextFile.close();
      }
    }
    break;
    case RNG:
    {
      randomSeed(micros());
      uint16_t randomFile = currentFileNumber;
      while(randomFile == currentFileNumber)
        randomFile = random(numberOfFiles-1);
      currentFileNumber = randomFile;
      SD.vwd()->rewind();
      nextFile.openNext(SD.vwd(), O_READ);
      {
        for(int i = 0; i<randomFile; i++)
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
      for(int i = 0; i<numberOfFiles; i++)
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
        return;
      }
    }
    break;
  }
  ym2612.Reset();
  sn76489.Reset();
  waitSamples = 0;
  loopOffset = 0;
  lastWaitData61 = 0;
  cachedWaitTime61 = 0;
  pauseTime = 0;
  startTime = 0;
  loopCount = 0;
  cmd = 0;
  ClearBuffers();
  Serial.print("Current file number: "); Serial.print(currentFileNumber+1); Serial.print("/"); Serial.println(numberOfFiles);
  if(vgm.isOpen())
    vgm.close();
  vgm = SD.open(fileName, FILE_READ);
  if(!vgm)
  {
    Serial.println("File open failed!");
  }
  else
    Serial.println("Opened successfully...");
  //FillBuffer();
  GetHeaderData();
  singleSampleWait = ((1000.0 / (sampleRate/(float)1))*1000);

    for(int i = 0; i<16; i++)
    {
      if(i == 0)
      {
        preCalced8nDelays[i] = 0;
        preCalced7nDelays[i] = 1;
      }
      else
      {
        preCalced8nDelays[i] = ((1000.0 / (sampleRate/(float)i))*1000);
        preCalced7nDelays[i] = ((1000.0 / (sampleRate/(float)i+1))*1000);
      }
    }
    if(rate == 60 || rate == 0)
    {
        ymClock.SetFrequency(7670453); //PAL 7600489 //NTSC 7670453
        snClock.SetFrequency(3579545); //PAL 3546894 //NTSC 3579545 
    }
    else
    {
        ymClock.SetFrequency(7600489); //PAL 7600489 //NTSC 7670453
        snClock.SetFrequency(3546894); //PAL 3546894 //NTSC 3579545 
    }
    delay(200);
}

bool buttonLock = false;
void loop()
{
  while(Serial.available())
  {
    bool USBorBluetooh = Serial.available();
    char serialCmd = Serial.read();
    switch(serialCmd)
    {
      case '+': //Next song
        StartupSequence(NEXT);
      break;
      case '-': //Previous Song
        StartupSequence(PREVIOUS);
      break;
      case '*': //Pick random song
        StartupSequence(RNG);
      break;
      case '/': //Toggle shuffle mode
        playMode == SHUFFLE ? playMode = IN_ORDER : playMode = SHUFFLE;
        playMode == SHUFFLE ? Serial.println("SHUFFLE ON") : Serial.println("SHUFFLE OFF");
        DrawOledInfo();
      break;
      case '.': //Toggle loop mode
        playMode == LOOP ? playMode = IN_ORDER : playMode = LOOP;
        playMode == LOOP ? Serial.println("LOOP ON") : Serial.println("LOOP OFF");
        DrawOledInfo();
      break;
      case 'r': //Song Request, format:  r:mySongFileName.vgm - An attempt will be made to find and open that file.
      {
        String req = Serial.readString();
        req.remove(0, 1); //Remove colon character
        StartupSequence(REQUEST, req);
      }
      break;
      case '?': //Send back information about the track
        Serial.println(trackTitle);
      break;
      case '!': //Toggle the OLED
        isOLEDOn = !isOLEDOn;
        DrawOledInfo();
      break;
    }
  }

  if(!digitalRead(option_btn) && !buttonLock)
  {
    PlayMode currentMode = playMode;
    if(playMode == SHUFFLE)
      playMode = LOOP;
    else if(playMode == LOOP)
      playMode = IN_ORDER;
    else if(playMode == IN_ORDER)
      playMode = SHUFFLE;
    while(!digitalRead(option_btn)) //Toggle the OLED by first holding the playmode button then hitting the random button.
    {
      if(!digitalRead(rand_btn))
      {
        isOLEDOn = !isOLEDOn;
        playMode = currentMode;
        break;
      }
    }
    DrawOledInfo();
    buttonLock = true;
  }

  if(!digitalRead(next_btn))
    StartupSequence(NEXT);
  if(!digitalRead(prev_btn))
    StartupSequence(PREVIOUS);
  if(!digitalRead(rand_btn) && !buttonLock)
    StartupSequence(RNG);


  if(loopCount >= nextSongAfterXLoops)
  {
    if(playMode == SHUFFLE)
      StartupSequence(RNG);
    if(playMode == IN_ORDER)
      StartupSequence(NEXT);
  }

  if(buttonLock)
  {
    if(digitalRead(option_btn) && digitalRead(rand_btn))
      buttonLock = false;
  }

  unsigned long timeInMicros = micros();
  if( timeInMicros - startTime <= pauseTime)
  {
    // Serial.print("timeInMicros"); Serial.print("\t"); Serial.println(timeInMicros);
    // Serial.print("DELTA"); Serial.print("\t"); Serial.println(timeInMicros - startTime);
    // Serial.print("startTime"); Serial.print("\t"); Serial.println(startTime);
    //Serial.print("pauseTime"); Serial.print("\t"); Serial.println(pauseTime);
    //delay(150);
    return;
  }
  
  cmd = vgm.read();
  switch(cmd)
  {
    case 0x4F:
    sn76489.SendDataPins(0x06);
    sn76489.SendDataPins(vgm.read());
    startTime = timeInMicros;
    pauseTime = singleSampleWait;
    case 0x50:
    sn76489.SendDataPins(vgm.read());
    startTime = timeInMicros;
    pauseTime = singleSampleWait;
    break;  
    case 0x52:
    {
    byte address = vgm.read();
    byte data = vgm.read();
    ym2612.SendDataPins(address, data, 0);
    }
    startTime = timeInMicros;
    pauseTime = singleSampleWait;
    break;
    case 0x53:
    {
    byte address = vgm.read();
    byte data = vgm.read();
    ym2612.SendDataPins(address, data, 1);
    }
    startTime = timeInMicros;
    pauseTime = singleSampleWait;
    break;
    case 0x61:
    {
      //Serial.print("0x61 WAIT: at location: ");
      //Serial.print(parseLocation);
      //Serial.print("  -- WAIT TIME: ");
      uint32_t wait = 0;
      for ( int i = 0; i < 2; i++ )
      {
        wait += ( uint32_t( vgm.read() ) << ( 8 * i ));
      }
    startTime = timeInMicros;
    pauseTime = ((1000.0 / (sampleRate/(float)wait))*1000);
    //delayMicroseconds(cachedWaitTime61);
    //delay(cachedWaitTime61);
    break;
    }
    case 0x62:
    startTime = timeInMicros;
    pauseTime = WAIT60TH;
    //delay(16.67);
    //delayMicroseconds(WAIT60TH); //Actual time is 16.67ms (1/60 of a second)
    break;
    case 0x63:
    startTime = timeInMicros;
    pauseTime = WAIT50TH;
    //delay(20);
    //delayMicroseconds(WAIT50TH); //Actual time is 20ms (1/50 of a second)
    break;
    case 0x67:
    {
      //Serial.print("DATA BLOCK 0x67.  PCM Data Size: ");
      vgm.read();
      vgm.read(); //Skip 0x66 and data type
      pcmBufferPosition = vgm.curPosition();
      uint32_t PCMdataSize = Read32();
      if(PCMdataSize > MAX_PCM_BUFFER_SIZE)
        StartupSequence(NEXT);
      //Serial.println(PCMdataSize);

      for ( uint32_t i = 0; i < PCMdataSize; i++ )
      {
        ram.WriteByte(i, vgm.read()); 
      }
      //Serial.println("Finished buffering PCM");
      break;
    }
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
      //Serial.println("0x7n WAIT");
      uint32_t wait = cmd & 0x0F;
      //Serial.print("Wait value: ");
      //Serial.println(wait);
      startTime = timeInMicros;
      pauseTime = preCalced7nDelays[wait];
    break;
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
    uint32_t wait = cmd & 0x0F;
    byte address = 0x2A;
    unsigned char data = ram.ReadByte(pcmBufferPosition++);
    ym2612.SendDataPins(address, data, 0);
    //Serial.print("RAM READ: "); 
    //Serial.println(data, HEX);
    startTime = timeInMicros;
    pauseTime = preCalced8nDelays[wait];
    break;
    }
    case 0xE0:
    {
      //Serial.print("LOCATION: ");
      //Serial.print(parseLocation, HEX);
      //Serial.print(" - PCM SEEK 0xE0. NEW POSITION: ");
      pcmBufferPosition = Read32();
      break;
    }
    case 0x66:
      if(loopOffset == 0)
        loopOffset = 0x40;
      loopCount++;
      vgm.seek(loopOffset-0x1C);
      bufferPos = 0;
      break;
      
      default:
      Serial.print("Defaulted command: "); Serial.println(cmd, HEX);
      Serial.print("At: "); Serial.println(vgm.position()-1, HEX);
      break;
  } 
}
void setup()
{
    pinMode(prev_btn, INPUT_PULLUP);
    pinMode(rand_btn, INPUT_PULLUP);
    pinMode(next_btn, INPUT_PULLUP);
    pinMode(option_btn, INPUT_PULLUP);

    Wire.begin();
    SPI.begin();
    ymClock.SetFrequency(7670453); //PAL 7600489 //NTSC 7670453
    snClock.SetFrequency(3579545); //PAL 3546894 //NTSC 3579545 
    Serial.begin(9600);
    ym2612.Reset();
    delay(500);
    u8g2.begin();
    u8g2.setFont(u8g2_font_fub11_tf);
    u8g2.clearBuffer();
    u8g2.drawXBM(0,0, logo_width, logo_height, logo);
    u8g2.sendBuffer();
    delay(5000);
    Serial.println("INIT GOOD!");

    if(!SD.begin())
    {
        Serial.println("Card Mount Failed");
        u8g2.clearBuffer();
        u8g2.drawStr(0,16,"SD Mount");
        u8g2.drawStr(0,32,"failed!");
        u8g2.sendBuffer();
        return;
    }
    ram.Init();
    RemoveSVI();
    File countFile;
    while ( countFile.openNext( SD.vwd(), O_READ ))
    {
      countFile.close();
      numberOfFiles++;
    }
    countFile.close();
    SD.vwd()->rewind();
    StartupSequence(FIRST_START);
}