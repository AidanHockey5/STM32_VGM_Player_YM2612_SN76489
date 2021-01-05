NO DEPENDANCIES OR PYTHON INSTALL REQUIRED FOR WINDOWS EXE



PYTHON DEPENDANCIES:
Python 3.0+
pyserial
requests

Enter the following commands:

pip install pyserial
pip install requests


Run:
python3 Flash.py

----------------------------------------------

Instructions for use:

On the MEGABLASTER DEVICE: Move the left jumper pin at the top of the board to the "UP" position.
It should look like this:
https://www.aidanlawrence.com/wp-content/uploads/2021/01/mb_jumper-scaled.jpg

Plug-in your MEGABLASTER to your computer via USB and apply 12V to the power jack.

Switch the unit ON.

Press the RESET BUTTON.

Run the Flash.exe program OR run Flash.py with Python.

Select the COM port of the MEGABLASTER.

If everything is correct, your system should automatically download the firmware file and flash.

After flashing, replace the jumper's position back to DOWN.

----------------------------------------------

EXE Built with pyinstaller
pyinstaller --onefile Flash.py