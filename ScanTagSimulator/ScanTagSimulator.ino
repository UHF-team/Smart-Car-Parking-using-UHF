#include "SerialTransfer.h"


SerialTransfer myTransfer;

struct Package {
  byte tagCode[12] = {0x34, 0x16, 0x21, 0x4B, 0x88, 0xF6, 0xBB, 0x00, 0x02, 0x66, 0x24, 0x03};
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
  delay(2000);
}
