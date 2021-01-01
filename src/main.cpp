#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "SdFat.h"
#include "U8g2lib.h" //Run this command in the terminal below if PIO asks for a dependancy: pio lib install "U8g2"
#include "LTC6904.h"
#include "YM2612.h"
#include "SN76489.h"
#include "logo.h"
#include "SpinSleep.h"
#include "SerialUtils.h"

#include "VGMEngine.h"

//Debug variables
#define DEBUG true //Set this to true for a detailed printout of the header data & any errored command bytes
#define DEBUG_LED PA8
bool commandFailed = false;
uint8_t failedCmd = 0x00;

//Structs
enum FileStrategy {FIRST_START, NEXT, PREV, RND, REQUEST};
enum PlayMode {LOOP, PAUSE, SHUFFLE, IN_ORDER};

//Prototypes
void setup();
void loop();
void handleSerialIn();
void tick();
void setISR();
void pauseISR();
void removeMeta();
void prebufferLoop();
void injectPrebuffer();
void fillBuffer();
bool topUpBuffer(); 
void clearBuffers();
void handleButtons();
void prepareChips();
void readGD3();
void drawOLEDTrackInfo();
bool startTrack(FileStrategy fileStrategy, String request = "");
bool vgmVerify();
uint8_t VgmCommandLength(uint8_t Command);
uint8_t readBuffer();
uint16_t readBuffer16();
uint32_t readBuffer32();
uint32_t readSD32();
uint16_t parseVGM();

LTC6904 ltcOPN(0);
LTC6904 ltcSN(1);

ChipClock opnClock(&ltcOPN);
ChipClock snClock(&ltcSN);

Bus bus(PB8, PB9, PC13, PC14, PC15, PA0, PA1, PA2);

YM2612 opn(&opnClock, &bus, PB3, NULL, PA12, PA11, PB0, PB4);
SN76489 sn(&snClock, &bus, PB5);

//OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0);
//U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0);
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0);
bool isOledOn = true;

//Buttons
const int prev_btn = PB11;
const int rand_btn = PB10;
const int next_btn = PB1;
const int option_btn = PA3;

//SD & File Streaming
SdFat SD;
File file;
#define MAX_FILE_NAME_SIZE 128
char fileName[MAX_FILE_NAME_SIZE];
uint32_t numberOfFiles = 0;
uint32_t currentFileNumber = 0;

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
PlayMode playMode = SHUFFLE;
bool doParse = false;

void setup()
{
  //DEBUG
  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, LOW);
  disableDebugPorts();
  resetSleepSpin();

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
  ltcOPN.SetFrequency(7670454); //PAL 7600489 //NTSC 7670453
  ltcSN.SetFrequency(3579545);  //PAL 3546894 //NTSC 3579545 

  //Set Chips
  VGMEngine.ym2612 = &opn;
  VGMEngine.sn76489 = &sn;

  //OLED
  oled.begin();
  oled.setFont(u8g2_font_fub11_tf);
  oled.drawXBM(0,0, logo_width, logo_height, logo);
  oled.sendBuffer();
  delay(3000);
  oled.clearDisplay();

  //SD
  if(!SD.begin(PA4, SD_SCK_HZ(F_CPU/2)))
  {
    Serial.println("SD Mount Failed!");
    oled.clearBuffer();
    oled.drawStr(0,16,"SD Mount");
    oled.drawStr(0,32,"failed!");
    oled.sendBuffer();
    while(true){Serial.println("SD MOUNT FAILED"); delay(1000);}
  }

  Serial.flush();

  //Prepare files
  removeMeta();

  File countFile;
  while ( countFile.openNext( SD.vwd(), O_READ ))
  {
    countFile.close();
    numberOfFiles++;
  }
  countFile.close();
  SD.vwd()->rewind();

  //Begin
  startTrack(FIRST_START);
}

void removeMeta() //Remove useless meta files
{
  File tmpFile;
  while ( tmpFile.openNext( SD.vwd(), O_READ ))
  {
    memset(fileName, 0x00, MAX_FILE_NAME_SIZE);
    tmpFile.getName(fileName, MAX_FILE_NAME_SIZE);
    if(fileName[0]=='.')
    {
      if(!SD.remove(fileName))
      if(!tmpFile.rmRfStar())
      {
        Serial.print("FAILED TO DELETE META FILE"); Serial.println(fileName);
      }
    }
    if(String(fileName) == "System Volume Information")
    {
      if(!tmpFile.rmRfStar())
        Serial.println("FAILED TO REMOVE SVI");
    }
    tmpFile.close();
  }
  tmpFile.close();
  SD.vwd()->rewind();
}

void setISR()
{
  Timer2.pause();
  Timer2.setPrescaleFactor(1);
  Timer2.setOverflow(1633);
  Timer2.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  Timer2.attachInterrupt(TIMER_CH2, tick);
  Timer2.refresh();
  Timer2.resume();   
}

void pauseISR()
{
  Timer2.pause();
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
    oled.drawStr(0,10, widetochar(VGMEngine.gd3.enTrackName));
    oled.drawStr(0,20, widetochar(VGMEngine.gd3.enGameName));
    oled.drawStr(0,30, widetochar(VGMEngine.gd3.releaseDate));
    oled.drawStr(0,40, widetochar(VGMEngine.gd3.enSystemName));
    String fileNumberData = "File: " + String(currentFileNumber+1) + "/" + String(numberOfFiles);
    char* cstr;
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

//Mount file and prepare for playback. Returns true if file is found.
bool startTrack(FileStrategy fileStrategy, String request)
{
  pauseISR();
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
      request.trim();
      char *cstr = &request[0u]; //Convert to C-string
      if(SD.exists(cstr))
      {
        file.close();
        strcpy(fileName, cstr);
        Serial.println("File found!");
      }
      else
      {
        Serial.println("ERROR: File not found! Continuing with current song.");
        goto fail;
      }
    }
    break;
  }

  if(SD.exists(fileName))
    file.close();
  file = SD.open(fileName, FILE_READ);
  if(!file)
  {
    Serial.println("Failed to read file");
    goto fail;
  }
  else
  {
    delay(100);
    if(VGMEngine.begin(&file))
    {
      printlnw(VGMEngine.gd3.enGameName);
      printlnw(VGMEngine.gd3.enTrackName);
      printlnw(VGMEngine.gd3.enSystemName);
      printlnw(VGMEngine.gd3.releaseDate);
      drawOLEDTrackInfo();
      setISR();
      return true;
    }
    else
    {
      Serial.println("Header Verify Fail");
      goto fail;
    }
  }

  fail:
  setISR();
  return false;
}

//Count at 44.1KHz
void tick()
{
  VGMEngine.tick();
}

//Poll the serial port
void handleSerialIn()
{
  while(Serial.available())
  {
    pauseISR();
    char serialCmd = Serial.read();
    switch(serialCmd)
    {
      case '+':
        startTrack(NEXT);
      break;
      case '-':
        startTrack(PREV);
      break;
      case '*':
        startTrack(RND);
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
        printlnw(VGMEngine.gd3.enGameName);
        printlnw(VGMEngine.gd3.enTrackName);
        printlnw(VGMEngine.gd3.enSystemName);
        printlnw(VGMEngine.gd3.releaseDate);
      break;
      case '!':
        isOledOn = !isOledOn;
        drawOLEDTrackInfo();
      break;
      case 'r':
      {
        String req = Serial.readString();
        req.remove(0, 1); //Remove colon character
        startTrack(REQUEST, req);
      }
      break;
      default:
        continue;
    }
  }
  Serial.flush();
  setISR();
}

//Check for button input
bool buttonLock = false;
void handleButtons()
{
  bool togglePlaymode = false;
  uint32_t count = 0;
  
  if(!digitalRead(next_btn))
    startTrack(NEXT);
  if(!digitalRead(prev_btn))
    startTrack(PREV);
  if(!digitalRead(rand_btn))
    startTrack(RND);
  if(!digitalRead(option_btn))
    togglePlaymode = true;
  else
    buttonLock = false;
  while(!digitalRead(option_btn))
  {
    pauseISR();
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
  {
    togglePlaymode = false;
    setISR();
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

    if(playMode == LOOP)
    {
      VGMEngine.maxLoops = 0xFFFF;
    }
    else
    {
      VGMEngine.maxLoops = maxLoops;
    }
    
    
    drawOLEDTrackInfo();
    setISR();
  }
}

void loop()
{    
  while(!VGMEngine.play()) //needs to account for LOOP playmode
  {
    if(Serial.available() > 0)
      handleSerialIn();
    handleButtons();
  }
  //Hit max loops and/or VGM exited
  if(playMode == SHUFFLE)
    startTrack(RND);
  if(playMode == IN_ORDER)
    startTrack(NEXT);
}
