# Smart Car Parking using UHF

## What is it ?
* To build a smart parking system which is more advanced than the existing parking system with a cheaper price than the market. 
* Using RFID (UHF) technology to identify vehicles by tags which are mounted on vehicles throughout the whole country.
## Required Boards and Peripherals
* Raspberry Pi 4 model B.
* Board UHF reader (ESP32 and M6e-Nano).
* Board UCA_Education (protocol Lora) version 3.9.
* Antenna RT-CP3-QL Lacuna edition
* Ultrasonic Sensor (not yet required in this version)
* Azure Blob Storage
## Hardware Installing
### Connection of ESP32 and M6e-Nano
* UHF Reader is a combination of ESP32 and M6e-Nano.                      
![image](https://user-images.githubusercontent.com/67703507/193775757-88c3719b-aa31-4a47-9f91-1dbac2676cc1.png)

* UHF Reader is based on other system built by ThomasThongCE and NguyenTranKha in advance.
![image](https://user-images.githubusercontent.com/67703507/193777272-29dcc159-28c4-4932-b7ac-355009a5fff2.png)
### Connection of UHF Reader and UCA Board (LoRa Sender)
![image](https://user-images.githubusercontent.com/67703507/193776798-4622b197-0f0a-4787-b856-2769e9f872f6.png)
### Connection of UCA Board (LoRa Receiver) and Raspberry PI
![image](https://user-images.githubusercontent.com/67703507/193778195-6770b4ee-ecd8-4d70-82c3-6af24076d8ea.png)

## Software Installing
### Install UCA Boards
1. Add `https://rfthings.github.io/ArduinoBoardManagerJSON/package_rfthings-avr_index.json` as an "Additional Board Manager URL"
2. Open the Boards Manager from the Tools -> Board menu and install "RFThings AVR Boards by RFThings Vietnam"
3. Select board from the Tools -> Board menu -> RFThings AVR Boards -> RFThings UCA
3. Select version from the Tools -> Board version -> 3.9 and newer (ATMega328PB)
### Install ESP32 Board
1. Add `https://dl.espressif.com/dl/package_esp32_index.json` as an "Additional Board Manager URL"
2. Open the Boards Manager from the Tools -> Board menu and install "ESP32"
3. Select board from the Tools -> Board menu -> RFThings AVR Boards -> ESP32 Dev Module
### Install Required Libraries
* Access https://github.com/sandeepmistry/arduino-LoRa#using-the-arduino-ide-library-manager to download and install LoRa library
* Access https://github.com/sparkfun/SparkFun_Simultaneous_RFID_Tag_Reader_Library to download and install UHF Reader library
* Access https://github.com/sparkfun/SparkFun_Simultaneous_RFID_Tag_Reader_Library to download and install SerialTransfer library
* Access https://github.com/PowerBroker2/pySerialTransfer to download and install SerialTransfer library
## Documentation
* [Official Document](https://github.com/UHF-team/Smart-Car-Parking-using-UHF/blob/main/Doc/Smart_Car_Parking_System_Document.pdf) - Entire information of smart car parking system.
* [Hookup Guide](https://github.com/UHF-team/Smart-Car-Parking-using-UHF/blob/main/Doc/Specification_UHF%20Reader.pdf) - Specification of the UHF Reader.
## Implementation Video
* [System Description Video](https://www.youtube.com/watch?v=sMzCV0ur1mA) - Video show the how smart car parking system works.
