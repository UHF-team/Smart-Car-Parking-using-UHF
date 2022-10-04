#include <LoRa.h>
#include <SPI.h>
#include "SerialTransfer.h"

//------------------- Configue Parameters --------------------//

#define UHF_ID_LENGTH 7
#define TAG_CODE_LENGTH 12
#define PAYLOAD_LENGTH 19

SerialTransfer myTransfer;
const long LoRa_freq = 915E6;
const int LoRa_SF = 9;
const long LoRa_bw = 125E3;

//------------------- Define package data --------------------//

struct Package {
  char uhfId[UHF_ID_LENGTH] = {'a', 'b', 'c', '1', '2', '3', '4'};
  byte tagCode[TAG_CODE_LENGTH];
} package;

//-------------- Send LoRa Message to Receiver ---------------//

void sendLoRaMessage(byte* message) {
  LoRa.beginPacket();
  for (int i = 0; i < PAYLOAD_LENGTH; i++)
  {
    Serial.print("0x");
    if(message[i] < 0x10) Serial.print("0");
    Serial.print(message[i], HEX);
    Serial.print(" ");
    
    LoRa.print(char(message[i]));
  }
  LoRa.endPacket();
}

//---------------------- Setup Devices -----------------------//

void setup() {
  
  // UART setup
  Serial.begin(9600);
  while (!Serial);
  myTransfer.begin(Serial);
  Serial.println("LoRa Sender");

  // LoRa setup
  if (!LoRa.begin(LoRa_freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(LoRa_SF);
  LoRa.setSignalBandwidth(LoRa_bw);
  Serial.println("- LoRa ready!");
}

//----------------------- Main Program -----------------------//

void loop() {

  // Save tagCode from M6E-NANO UART data
  while (!myTransfer.available());
  myTransfer.rxObj(package.tagCode);

  // Merge UHF device ID and tagCode to send LoRa message
  byte message[PAYLOAD_LENGTH];
  for(int i = 0; i < UHF_ID_LENGTH; i+=1){
    message[i] = package.uhfId[i];
  }
  for(int i = 0; i < TAG_CODE_LENGTH; i+=1){
    message[i+UHF_ID_LENGTH] = package.tagCode[i];
  }
  sendLoRaMessage(message);

  // Check if sended package is error
  int nackCounter = 0;
  while (!receiveAck(message) && nackCounter < 5) {
    Serial.println(" - refused ");
    Serial.println();

    nackCounter++;
    Serial.print("Nack = "); 
    Serial.print(nackCounter);
    Serial.print(" - ");
    delay(1000);
    sendLoRaMessage(message);
  }
  
  if (nackCounter >= 5) {
    Serial.println("");
    Serial.println("#----- MESSAGE LOST -----#");
    delay(100);
  } else {
    Serial.println(" - Acknowledged ");
    Serial.println();
    myTransfer.reset();
  }
  
  delay(100);
}

//--------------- Receive ACK checksum message ---------------//

bool receiveAck(byte* message) {
  String ack;
  Serial.print("Waiting for Ack ");
  bool stat = false;
  unsigned long entry = millis();
  while (stat == false && millis() - entry < 2000) {
    if (LoRa.parsePacket()) {
      ack = "";
      while (LoRa.available()) {
        ack = ack + ((char)LoRa.read());
      }
      
  int check = 0;   
  for (int i = 0; i < PAYLOAD_LENGTH; i++){
    check += message[i];
  }
    
      Serial.print("[ Ack = ");
      Serial.print(ack);
      Serial.print(" - Check = ");
      Serial.print(check);
      Serial.print("]");
      if (ack.toInt() == check) {
        Serial.print(" - Checksum OK ");
        stat = true;
      }
    }
  }
  return stat;
}
