#include <SPI.h>
#include <LoRa.h>
#include "SerialTransfer.h"

//------------------- Configue Parameters --------------------//                                                              

#define PAYLOAD_LENGTH 19
const long LoRa_freq = 915E6;
const int LoRa_SF = 9;
const long LoRa_bw = 125E3;
SerialTransfer myTransfer;

//---------------- Send back LoRa ACK packet -----------------//

void sendACK(byte *message) {
  int check = 0;
  for (int i = 0; i < PAYLOAD_LENGTH; i++) {
    check += message[i];
  }
  
  LoRa.beginPacket();
  LoRa.print(String(check));
  LoRa.endPacket();
  
  Serial.print(" ACK Sent: ");
  Serial.println(check);
}

//--------------- Send Message to PI via UART ----------------//

void sendUARTMessage(byte *message) {
  for (int i = 0; i <= PAYLOAD_LENGTH; i++)
    myTransfer.packet.txBuff[i]= message[i];
  
  myTransfer.sendData(PAYLOAD_LENGTH);
}

//-------------- Send LoRa Message to Receiver ---------------//

void receiveMessage() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    byte message[PAYLOAD_LENGTH];
    int i = 0;
    while (LoRa.available()) {
      message[i] = LoRa.read();
      Serial.print(message[i], HEX);
      i++;
    }
    sendACK(message);
    sendUARTMessage(message);
    }
}

//---------------------- Setup Devices -----------------------//

void setup() {

  // UART setup
  Serial.begin(115200);
  while (!Serial);
  myTransfer.begin(Serial);
  Serial.println("Receiver");

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
  receiveMessage();
}
