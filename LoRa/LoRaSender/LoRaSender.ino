#include <LoRa.h>
#include <SPI.h>
#include "SerialTransfer.h"

#define UHF_ID_LENGTH 7
#define TAG_CODE_LENGTH 12
#define PAYLOAD_LENGTH 19

// ---------- Configuration parameters ---------- //
SerialTransfer myTransfer;
const long freq = 915E6;
const int SF = 9;
const long bw = 125E3;

// ---------- Define package data ---------- //
struct Package {
  char uhfId[UHF_ID_LENGTH] = {'a', 'b', 'c', '1', '2', '3', '4'};
  byte tagCode[TAG_CODE_LENGTH];
  //uint8_t presentTime[3];
} package;

// ---------- Setup devices ---------- //
void setup() {
  Serial.begin(9600);
  while (!Serial);
  myTransfer.begin(Serial);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(SF);
  // LoRa.setSignalBandwidth(bw);
  Serial.println("- LoRa ready!");
}

// ---------- Main program ---------- //
void loop() {
  while (!myTransfer.available());
  myTransfer.rxObj(package.tagCode);

// ---------- Merge uhfId and tagCode to send LoRa ---------- //  
  byte message[PAYLOAD_LENGTH];
  for(int i = 0; i < UHF_ID_LENGTH; i+=1){
    message[i] = package.uhfId[i];
  }

  for(int i = 0; i < TAG_CODE_LENGTH; i+=1){
    message[i+UHF_ID_LENGTH] = package.tagCode[i];
  }
  
  sendMessage(message);

// ---------- Check whether the sended package is error ---------- //  
  int nackCounter = 0;
  while (!receiveAck(message) && nackCounter < 5) {
    Serial.println(" - refused ");
    Serial.println();

    nackCounter++;
    Serial.print("Nack = "); 
    Serial.print(nackCounter);
    Serial.print(" - ");
    delay(1000);
    sendMessage(message);
  }
  
  if (nackCounter >= 5) {
    Serial.println("");
    Serial.println("--------------- MESSAGE LOST ---------------------");
    delay(100);
  } else {
    Serial.println(" - Acknowledged ");
    Serial.println();
    myTransfer.reset();
  }
  
  delay(100);
}

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

void sendMessage(byte* message) {
  Serial.print("[");
  for(int i = 0; i < PAYLOAD_LENGTH; i++)
  {
    Serial.print(" 0x");
    if(message[i] < 0x10) Serial.print("0");
    Serial.print(message[i], HEX);
  }
  Serial.println(" ]");
  
  LoRa.beginPacket();
  for (int i = 0; i < PAYLOAD_LENGTH; i++)
  {
    LoRa.print(char(message[i]));
  }
  LoRa.endPacket();
}
