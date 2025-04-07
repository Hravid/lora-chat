## ABOUT

Web app that uses LoRa + ESP32 to chat with others the *safe* (?) way.

Hardware components required:
1. ESP32
2. LoRa module
3. SD card reader module
4. Antena 

## ESP32 + LoRa + SD

I just ordered - https://wiki.seeedstudio.com/wio_sx1262_with_xiao_esp32s3_kit/
The second i receive package im gonna test range capabilities, probably gonna switch antena. Kit is super compact makes it super easy to carry in a bag.
Probbaly gonna add GPS module to it.
Right now messages are stored on SD card (currently 1 message for testing purposes).

## CODE


## CONNECTING SD CARD TO ESP-WROOM-32

|ESP32|SD READER|
|-----|---------|
|3.3V |3.3V     |
|GND  |GND      |
|D5   |CS       |
|D18  |MOSI     |
|D19  |SCK      |
|D23  |MISO     |

## TO-DO

- message search engine
- add login capabilities
- create UI/UX
- determine if contacts should be stored on server or sd card
- data encryption
- range tests
- capability to transfer files other than text
- add gps module

