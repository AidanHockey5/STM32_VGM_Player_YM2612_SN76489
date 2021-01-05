#This script will automatically grab the latest MegaBlaster firmware from github and install it.
#You need to place the MegaBlaster into programming mode BEFORE you invoke this tool

#There are two jumpers on the STM32 board.
#The left jumper needs to straddle the upper pin and middle pin
#The right jumper needs to straddle the lower pin and middle pin

#You need to ALSO hit the reset button AFTER you have set the jumpers

#Once programming is complete, place the left jumper back to it's low-middle pin position.

import sys
import re
import glob
import serial
import ssl
import requests
from multiprocessing import Queue
import json
import urllib
import certifi
import os
from io import StringIO
requests.packages.urllib3.disable_warnings()

def SSLCheck():
    try:
        _create_unverified_https_context = ssl._create_unverified_context
    except AttributeError:
        # Legacy Python that doesn't verify HTTPS certificates by default
        pass
    else:
        # Handle target environment that doesn't support HTTPS verification
        ssl._create_default_https_context = _create_unverified_https_context
    
def GetOS():
    os = ""
    if sys.platform.startswith('win'):
        os = "WIN"
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        os = "LIN"
    elif sys.platform.startswith('darwin'):
        os = "OSX"
    else:
        raise EnvironmentError('Unsupported platform')
    return os
    
if getattr(sys, 'frozen', False):
    # If the application is run as a bundle, the PyInstaller bootloader
    # extends the sys module by a flag frozen=True and sets the app 
    # path into variable _MEIPASS' or executable if onefile.
    SCRIPT_LOCATION = sys.executable
    SCRIPT_LOCATION = re.sub('\\\Flash.exe', '', SCRIPT_LOCATION)
else:
    SCRIPT_LOCATION = os.path.dirname(os.path.abspath(__file__))

OPERATING_SYSTEM = GetOS()

#= sys.path[0]
AVRDUDE_LOCATION = ""
if(OPERATING_SYSTEM == "WIN"):
    AVRDUDE_LOCATION = SCRIPT_LOCATION+"\\FLASH_FIRMWARE"
elif(OPERATING_SYSTEM == "LIN" or OPERATING_SYSTEM == "OSX"):
    AVRDUDE_LOCATION = SCRIPT_LOCATION+"/FLASH_FIRMWARE"
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if OPERATING_SYSTEM == "WIN":
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif OPERATING_SYSTEM == "LIN":
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif OPERATING_SYSTEM == "OSX":
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

if(OPERATING_SYSTEM == "OSX"):
    SSLCheck() #MacOS gets the nuclear option for being dumb with ssl

print("---MegaFlasher Python MegaBlaster Edition---")
print("This script will detect your STM32 serial port and invoke stm32flash for you.")
print("===================")
print("")

inputValid = False
ports = serial_ports()
selectedPort = 0
while inputValid == False:
    print("Please select your serial port by typing-in the boxed number that preceeds it")
    ports = serial_ports()

    if(len(ports) == 0):
        print("No serial devices found. Please connect your programmer and reload this program.")
        input()
        sys.exit()

    i = 0
    for port in ports:
        print("[" + str(i) + "] " + port)
        i += 1
    selectedPort = int(input("-->"))
    if(selectedPort < 0 or selectedPort > i):
        print("Invalid selection, please try again")
    else:
        inputValid = True
        print("OK")
        print(ports[selectedPort] + " selected")

print("Fetching latest firmware...")

r = urllib.request.urlopen('https://api.github.com/repos/AidanHockey5/STM32_VGM_Player_YM2612_SN76489/releases')

if(r.getcode() == 200):
    repoItems = json.load(r)
    latest = repoItems[0]["assets"][0]["browser_download_url"]
    print(latest)
    if(OPERATING_SYSTEM == "WIN"):
        urllib.request.urlretrieve (latest, AVRDUDE_LOCATION+"\\firmware.bin")
    elif(OPERATING_SYSTEM == "LIN" or OPERATING_SYSTEM == "OSX"):
        urllib.request.urlretrieve (latest, AVRDUDE_LOCATION+"/firmware.bin")
    print("OK")
    print("Invoking STM32Flash, please wait...")

    if(OPERATING_SYSTEM == "WIN"):
        os.system(AVRDUDE_LOCATION+"\\stm32flash.exe -w ./FLASH_FIRMWARE/firmware.bin -v -g 0x0 " + ports[selectedPort])
    elif(OPERATING_SYSTEM == "LIN"):
        os.system(AVRDUDE_LOCATION+"\\/stm32flash -w ./FLASH_FIRMWARE/firmware.bin -v -g 0x0 " + ports[selectedPort])
    elif(OPERATING_SYSTEM == "OSX"):
        os.system(AVRDUDE_LOCATION+"\\/OSX/stm32flash -w ./FLASH_FIRMWARE/firmware.bin -v -g 0x0 " + ports[selectedPort])
else:
    print("Github API request failed. Please try again.")
    input()
    sys.exit()
