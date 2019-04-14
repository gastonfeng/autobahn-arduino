# autobahn-arduino

## Requirement
This README assumes that Arduino IDE is already installed in your PC.

## Install ESP32 toolchain
Follow the instruction as shown in the link.
https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/debian_ubuntu.md
############################# Info from the link ###################################
sudo usermod -a -G dialout $USER && \
sudo apt-get install git && \
wget https://bootstrap.pypa.io/get-pip.py && \
sudo python get-pip.py && \
sudo pip install pyserial && \
mkdir -p ~/Arduino/hardware/espressif && \
cd ~/Arduino/hardware/espressif && \
git clone https://github.com/espressif/arduino-esp32.git esp32 && \
cd esp32 && \
git submodule update --init --recursive && \
cd tools && \
python2 get.py
###############################################################################
## Install dependancy
After installing the esp32 toolchain, open the Arduino IDE and install the dependancies for Autobahn as instructed below.

Sketch->IncludeLibraries->Manage Libraries
Select Arduinojson by Benoit Blachon version 5.13.2 (Note that beta 6 version will not work)
Select Websocket by Markus sattlet version 2.1.1

## Dowload the Autobahn source:
[https://github.com/codelectron/autobahn-arduino/](https://github.com/codelectron/autobahn-arduino/)
## Install Autobahn:
mkdir -p $HOME/Arduino/libraries/Autobahn/
cp -a autobahn-arduino/src/* $HOME/Arduino/libraries/Autobahn/
## Examples:
Open the .ino file from the Arduino IDE, Configure the Wifi credentials, IP address, port and realm of your Crossbar.io instance and flash it in your esp32 board.

[https://github.com/codelectron/autobahn-arduino/tree/master/examples/esp32](https://github.com/codelectron/autobahn-arduino/tree/master/examples/esp32)

