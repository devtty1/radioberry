radioberry
==========

Internetradio with lcd and analog control devices.

This application is designed to run on a radioberry Pi(R) board
(http://www.raspberrypi.org/). The application provides functionalities to
control the volume and stations using potentiometers and displays station
information on a LC-Display. The potentionmeters shall be connected to an
ADC. At time of writing the mcp32XX serie ADCs on SPI bus are supported.
Extension to other ADC and bus connection shall be easy though. The LC
Display support shall run with any hd44780 compatible devices. The device is
connected directly via GPIO. At time of writing no serial/I2C connected LCDs
are supported. Extension here is also possible.

To use this application, you should have mpd (music player daemon, see
http://mpd.wikia.com/wiki/Music_Player_Daemon_Wiki) installed. Following
libraries are needed:
* libasound3
* libconfuse
* libmpd

This application is developed and tested on a Raspberry board. However it
should run on any embedded machin with SPI and GPIO interfaces.
