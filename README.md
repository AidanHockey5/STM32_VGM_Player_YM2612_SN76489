# Mega Blaster

See this project in action here: https://www.youtube.com/watch?v=WoAp2-gWaLM

This project is a hardware Video Game Music (VGM) player that uses a genuine YM2612 synthesizer chip + SN76489AN PSG. 
This project is driven by an STM32 "Blue Pill" board, chosen for it's ample speed, I/O, and best of all, price!

Included in this repository is the source code for the project, all of the schematic files, and a completed 
Ki-CAD printed circuit board /w Gerber files. Feel free to produce your own board!

![PCB](http://www.aidanlawrence.com/wp-content/uploads/2018/05/Thumbnail-1024x576.jpg)

# Information about the main sound chips and VGM

The YM2612 is a 6-channel FM synthesizer IC that was most prominently featured in the Sega Genesis (AKA Megadrive) home video game console. The YM2612 was also featured in the FM-Towns home computer and the Sega C2 Arcade System.
The YM2612 also had a CMOS alias that is functionally identical named the YM3438.
The SN76489 is a 3-channel + 1 noise channel programmable sound generator (PSG) that was popular with several early video game systems and home computers.
Most notably, the SN76489 was featured in the Sega Master system, the predecessor to the Genesis. To allow for backwards compatibility, Sega included a clone of the SN76489 in their new Genesis consoles.
Programmers and sound designers could leverage the SN76489 to add three more square wave channels to the YM2612's 6-channels of FM synth, creating 9 channels + 1 noise channels in total.
While this player was designed with the Genesis in mind, it can play back any VGM files designed for the Master System, FM-Towns PC, C2 machine, or any other machine that used the YM2612/YM3438 and/or the SN76489.
This project synthesizes music from .VGM files in real time on genuine hardware. There is no emulation here.

VGM stands for Video Game Music, and it is a 44.1KHz logging format that stores real soundchip register information. My player will parse these files and send the data to the appropriate chips. You can learn more about the VGM file format here: http://www.smspower.org/Music/VGMFileFormat

http://www.smspower.org/uploads/Music/vgmspec170.txt?sid=58da937e68300c059412b536d4db2ca0

# SD Card Information
This project is built for full-sized SD cards, but you may use adapters to fit your desired card. You must format your SD card to Fat32 in order for this device to work correctly. Your SD card must only contain uncompressed .vgm files. VGZ FILES WILL NOT WORK! You may download .vgz files and use [7zip](http://www.7-zip.org/download.html) to extract the uncompressed file out of them. Vgm files on the SD card do not need to have the .vgm extension. As long as they contain valid, uncompressed vgm data, they will be read by the program regardless of their name.
You can find VGM files by Googling "myGameName VGM," or by checking out sites like http://vgmrips.net/packs/

# Control Over Serial
You can use a serial connection to control playback features. The commands are as follows:

Command | Result
------------ | -------------
\+ | Next Track
\- | Previous Track
\* | Random Track
\/ | Toggle Shuffle Mode
\. | Toggle Song Looping
r: | Request song
? | What song is playing?
\! | Toggle the OLED display

A song request is formatted as follows: ```r:mySongFile.vgm```
Once a song request is sent through the serial console, an attempt will be made to open that song file. The file must exist on the SD card, and spelling/capitalization must be correct.
Need an easy-to-use serial console? [I've made one here.](https://github.com/AidanHockey5/OpenArduinoSerialConsole)

# Schematic
![Schematic](https://raw.githubusercontent.com/AidanHockey5/STM32_VGM_Player_YM2612_SN76489/master/Schematics/STM32_MegaBlaster/STM32_MegaBlaster.png)

[PDF](https://github.com/AidanHockey5/STM32_VGM_Player_YM2612_SN76489/raw/master/Schematics/STM32_MegaBlaster/STM32_MegaBlaster.pdf)
