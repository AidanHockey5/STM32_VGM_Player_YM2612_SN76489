// modified from ArduinoTestSuite 0022 by William Greiman
#include <SPI.h>
#include <SdFat.h>
#include <SdFatTestSuite.h>
SdFat SD;
#define ATS_PrintTestStatus(msg, b) testVerify_P(b, PSTR(msg))

void setup() {
  boolean b;
  SdFile f;

  testBegin();

  ATS_PrintTestStatus("SD.begin()", b = SD.begin());
  if (!b) goto done;

  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));
  ATS_PrintTestStatus("SD.open()", f.open("asdf.txt", FILE_WRITE)); f.close();
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf.txt"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("/asdf.txt"));
  ATS_PrintTestStatus("SD.remove()", SD.remove("asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));

  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));
  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("/asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));

  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("x/y/z"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/z"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y/z/"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("/x/y/z/"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("x/y/z"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x/y"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("x/y/"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("x"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("/x"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("x/y/z"));

  ATS_PrintTestStatus("!SD.open()", !(f.open("asdf/asdf.txt", FILE_WRITE))); f.close();
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.mkdir()", SD.mkdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.open()", f.open("asdf/asdf.txt", FILE_WRITE)); f.close();
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("!SD.rmdir()", !SD.rmdir("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.remove()", SD.remove("asdf/asdf.txt"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf/asdf.txt"));
  ATS_PrintTestStatus("SD.exists()", SD.exists("asdf"));
  ATS_PrintTestStatus("SD.rmdir()", SD.rmdir("asdf"));
  ATS_PrintTestStatus("!SD.exists()", !SD.exists("asdf"));

done:

  testEnd();

}
void loop() {}
