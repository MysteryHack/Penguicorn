# TESTING

This map contains al the materials you need to test the nrf24

## What you need
### Software
```
- Arduino ide
- Esp8266 Board
- NRF24 Lib from TMRh20
```
### Hardware
```
- Penguicorn badg
- arduino uno
- nrf24l01 board
```
## How to set-up

install the nrf24pingTestArduino file on the arduino with default settings.
when that is done you can move on to the penguicorn

open the nrf24pingTestPenguicorn file and set the board settings to a wemos d1
when the file is on the penguicorn you open the serial monitor on 115200 and type T then the you should se stats on the oled.

## How to use
use T and R on the arduino and the penguicorn to switch modes.
T for transmit
R for receive

Make sure when one of the two is in T the other one should be in R mode.

you can check if they are in the right mode by checking the oled and the leds.
|-----------------------------------|
|Mode	|   	Oled		|	  Led	      	|
|-----------------------------------|
|	T	  | 	transmit	|	  Blue      	|
|	R	  | 	Receive		|	  Green	      |
|-----------------------------------|

###Note
in transmit mode the leds can turn red this is to show that the message timeout has been trigged tis is also on the oled viable.
