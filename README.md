#  ADB to USB converter for Apple keyboards

Allow one ADB keyboard to be used on one ADB computer (old Macintosh and Apple IIgs) and one USB computer at the same time by switching between them.

![Illustration](illustration.jpg)

## What works

- adb passthrough between a macintosh and keyboard/mouse + softpower
- adb keyboard to usb-hid conversion:
  - iso and ansi layouts
  - locking caps lock
  - power key in usb (translated as 0x66 "keyboard power")
- tested on multiple keyboards: aek ii, m0116/m0118, appledesign...
- tested on macintosh se

## What don't work yet

- keyboard leds on usb: the *keyboard* library doesn't handle hid output reports
- hotplugging adb: works randomly, the microcontroller don't seems to like that
- mouse passthrough in usb-hid mode
- correct keyboard detection by the macintosh when the board is in usb mode: not a problem in practice
- collision detection handling: the program calls the keyboard at address 2

## Hardware needed

- arduino pro micro because it works on 5V and is hid capable
- 2 mini-din 4 pin connectors (only one if using only adb-to-usb)
- a resistor (1K) for the pull up
- a button (optional if using only adb-to-usb)
- a diode (optional, to power the arduino from adb)

## How to build it

- connect button between 2 and GND
- connect 9 to the mini-din A pin 1 (computer adb data)
- connect 8 to the mini-din A pin 2 (computer adb softpower)
- connect mini-din A pin 3 to RAW through the diode (pass from mini-DIN to RAW)
- connect 10 to the mini-din B pin 1 (keyboard adb data) then to VCC through resistor
- connect 16 to the mini-din B pin 2 (keyboard adb softpower)
- connect RAW to the mini-din B pin 3 (keyboard 5V power)
- connect GND on the two mini-din pins 4
- mark din A as the macintosh connector and din B as the keyboard connector (put a sticker or whatever)

Mini-DIN female connector front view :
```
/------\
| 4  3 |
| 2  1 |
\__==__/
```

Arduino Pro Micro pinout :
```
                                ___________ 
                     PD3  TXD  |    USB    |  RAW        --- keyboard pin 3 ---
                     PD2   20  |           |  GND        --- keyboard pin 4    |
computer pin 4 ---        GND  |           |  RST                              |
  button pin 1 ---        GND  |           |  VCC                              |
  button pin 2 ---   PD1    2  |           |  A3   PF4                         |
                     PD0    3  |           |  A2   PF5                         _
                     PD4    4  |           |  A1   PF6                        | |
                     PC6    5  |           |  A0   PF7            1K resistor | |
                     PD7    6  |           |  15   PB1                        |_|
                     PE6    7  |           |  14   PB3                         |
computer pin 2 ---   PB4    8  |           |  16   PB2   --- keyboard pin 2    |
computer pin 1 ---   PB5    9  |___________|  10   PB6   --- keyboard pin 1 ---
computer pin 3 --->| RAW
```

## How to compile and upload it

1. install `arduino-cli`
2. `arduino-cli core install arduino:avr`
3. `arduino-cli lib install Keyboard`
4. compile this project by `cd` in then `arduino-cli compile --fqbn arduino:avr:micro`
5. connect the board to the computer
6. get board port with `arduino-cli board list`
7. upload it `arduino-cli upload -p /dev/.......`

## References

- [Guide to the Macintosh family hardware, Chapter 8 : Apple Desktop Bus, Apple, 1990](https://archive.org/details/apple-guide-macintosh-family-hardware/page/n325/mode/2up)
- [Apple Desktop Bus Protocol, lopaciuk.eu, 2021](https://www.lopaciuk.eu/2021/03/26/apple-adb-protocol.html)
- [Understanding the ADB Service Request Signal, bigmessowires, 2016](https://www.bigmessowires.com/2016/03/30/understanding-the-adb-service-request-signal/)
- [Apple Desktop Bus (ADB), wiki.preterhuman.net](https://wiki.preterhuman.net/Apple_Desktop_Bus_(ADB))
