#include "SerialTransfer.h"


SerialTransfer myTransfer;

struct Package {
  byte tagCode[12] = {0x34, 0x16, 0x21, 0x4B, 0x88, 0xD5, 0x2B, 0x00, 0x01, 0x03, 0x90, 0x38};
} package;

void setup()
{
  Serial.begin(9600);
  //Serial1.begin(115200);
  myTransfer.begin(Serial);
}


void loop()
{
  // use this variable to keep track of how many
  // bytes we're stuffing in the transmit buffer
  //uint16_t sendSize = 0;

  ///////////////////////////////////////// Stuff buffer with struct
  //sendSize = myTransfer.txObj(testStruct, sendSize);

  ///////////////////////////////////////// Stuff buffer with array
  //sendSize = myTransfer.txObj(arr, sendSize);

  ///////////////////////////////////////// Send buffer
  myTransfer.sendDatum(package.tagCode);
  while(1);
  delay(2000);
}
