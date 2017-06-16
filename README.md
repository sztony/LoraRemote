# LoraRemote

This is a simple, Lora based, long range remote control switch, written in Arduino.  
It is currently optimized for the Adafruit Feather M0 Lora 433Mhz Module, but it can be used if modified for any appropriate Arduino+Lora Module combination.

Board: <https://www.adafruit.com/product/3179>   
can be bought from various distributers around the globe.  
Startup guide: <https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module>  
NOTE: please use the RadioHead library in this repository!

## Notes:

* The Code uses a modified version of the RadioHead and RTCZero Library, hence those two are include in this Repository. Make sure you have copied them into your Arduino Library Folder
	* Modifications:
		* RadioHead: Small bugfix for using the CAD function correctly
		* RTCZero: Modified, so that internal RTC Clock reference is used 
* **IMPORTANT:** Once you've uploaded the receiver code the first time, you will notice, that any further upload will fail. That's because the module is now sleeping for most of the time and hence it will not respond to any serial update command. To upload anyway, push the reset button twice until the red LED starts slowly fading. This might require some practice, but after a while you'll get the hang of it.
* The receiver code is written to consume as little power as possible. In return it can occasionally happen, that the switch command is not received at the first time. This will be further improved in the future, but for now, just press the sending button a little longer than needed to send the switch commonds more than once.
* One switch command will need about 1 second to be transmitted, you will notice this by the amount of time the SENDLED is on. Pressing the button longer than this one second will resend the packet immediately.
* **Power consumption** : Hard to tell at the moment, futher meassurements are needed. For the moment I would suggest ~1000 mAh per week, but very likely it is much less than this 
* **Range** : A lot of testing needed here, too. But our initial tests were pretty amazing: about 1,2 km in Urban enviroment through a lot of buildings, Line of sight will be much more (like ~10km or so)

## Connection
![PINOUT FEATHER M0](https://cdn-learn.adafruit.com/assets/assets/000/041/288/original/feather_3179_pinout.png?1493319154)
### Receiver
* Battery: you can connect any 1S Lipo/LiIon Battery cell to the Bat pin or battery connector. Or you connect 5V to the USB or VBUS pin
* Currently the switch output is connected to pin **12**
* if you want to switch a Relais or something, i would suggest you use a photomos relais like the [AQV252G](https://www.conrad.de/de/photomos-relais-1-st-panasonic-aqv252g-1-schliesser-60-vdc-60-vac-25-a-polzahl-6-504861.html). It is basically like an Relais, but without all the mechanics. The connection is pretty straightforward:
	* Pin 1 of the AQV252G to a 330 Ohm resistor and than to pin **12**
	* Pin 2 of the AQV252G to GND
	* The Load is switched between the Pins 4 and 6 of the AQV252G
* It can switch up to 2.5 Amps 60V DC! If you need more, or want to directly switch an 230V AC Source, just use the AQV252G to switch a "real" relais.

### Transmitter
* Battery: same as Receiver
* The connections of the Buttons can be set in the transmitter sketch. You always should connect one pin of the button to the desired pin number, the other pin to GND
* Any LED should be connected, so that the cathode is connected to GND, and the Anode over a 330 Ohm resister to the specified LED pin. **Always connect any LED with a resistor in between!!!**

## Further Improvements
* Enable two way communication with acknowledgement of the command reception
* Status request with current battery status
* More than one switch per Receiver
* More simple option to add multiple Receivers
* RTC based sleep for x days to begin with
* RTC based power on times for really low power consumption
* Further power reduction of the microcontroller