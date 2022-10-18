#include <SparkFun_UHF_RFID_Reader.h> // Library for controlling the M6E Nano module
#include "SerialTransfer.h"

//----------------- Configuration parameters -----------------//

RFID nano;
SerialTransfer myTransfer;
#define PRMDEBUG 0
#define WAIT_AFTER_POWER_ON 2000
#define SERIAL 9600
#define RFID_SERIAL_SPEED 19200
#define RFID_Region REGION_NORTHAMERICA // Valid options are :  REGION_INDIA, REGION_JAPAN, REGION_CHINA, REGION_EUROPE, REGION_KOREA, REGION_AUSTRALIA, REGION_NEWZEALAND, REGION_NORTHAMERICA
#define RFID_POWER 2700                 // 5.00 dBm. Max Read TX Power: 27.00 dBm and may cause temperature-limit throttling and USB port to brown out.
#define NANO_PARAMETER 0
#define TAG_CODE_LENGTH 12

int threshold = 40;

int detectTouch = 0;
int isStartReading = 0;
int isStopReading = 0;
int isScanning = 1;
//------------------- Define data package --------------------//

struct Package {
  byte tagCode[TAG_CODE_LENGTH];      // Example for a tag code {0x34, 0x16, 0x21, 0x4B, 0x88, 0xF6, 0xBB, 0x00, 0x02, 0x66, 0x24, 0x03};
} package, lastPackage;

//------------ Connect and setup M6e Nano Reader -------------//

int setupNano(long baudRate)
{
  if (PRMDEBUG == 3) nano.enableDebugging(Serial);

  nano.begin(Serial2); 
  Serial2.begin(baudRate);

  // Try to connect 50 times
  uint8_t val1 = 0;
  while(val1 < 50)
  {
    nano.getVersion();
    Serial.print(".");
    
    if (nano.msg[0] == ERROR_WRONG_OPCODE_RESPONSE )
    {
      if (PRMDEBUG > 1) Serial.println(F("Module continuously reading. Not Asking it to stop..."));
      return(2);
    }

    else if (nano.msg[0] != ALL_GOOD)
    {
      if (PRMDEBUG > 1) Serial.println(F("Try reset RFID speed"));

      Serial2.begin(115200);
      nano.setBaud(115200);
    }
    else
    {
      return(1); // Responded, but have to be set in the right mode
    }

    val1++;
  }

  return(0);
}

void setup_UHF() {
 
  if (setupNano(RFID_SERIAL_SPEED) == false)
  {
    Serial.println(F("Module failed to respond. Please check wiring."));
    while (1); //Freeze!
  }
  
  nano.setRegion(RFID_Region); // Set to Europe
  nano.setTagProtocol();       // Set protocol to GEN2
  nano.setAntennaPort();       // Set TX/RX antenna ports to 1
  nano.setReadPower(RFID_POWER);
  nano.startReading(); //Begin scanning for tags
  //nano.stopReading();
}

//---------------------- Setup Devices -----------------------//

void setup() {
  // If M6E Nano is powered with USB power, give the Nano time to settle.
  delay(WAIT_AFTER_POWER_ON);

  // Serial 1 setup
  Serial.begin(SERIAL);
  myTransfer.begin(Serial);
  while (!Serial); //Wait for the serial port to come online
  Serial.println("");
  
  // Nano setup
  Serial.println("RFID Init.");
  setup_UHF();
  Serial.println("RFID Done!");

  // Interrupt setup
  touchAttachInterrupt(T8, gotTouch, threshold);

  //
  detectTouch = 0;
  isStopReading = 0;
}

//----------------------- Main Program -----------------------//

void loop() {   
  if (nano.check() == true) //Check to see if any new data has come in from module
  {
    byte responseType = nano.parseResponse(); //Break response into tag ID, RSSI, frequency, and timestamp

    if (responseType == RESPONSE_IS_KEEPALIVE) {
      Serial.println(F("Scanning"));
    }
    else if (responseType == RESPONSE_IS_TAGFOUND) {
      
      // Send data package via UART if not a previous TAG
      for (byte x = 0 ; x < TAG_CODE_LENGTH ; x++) {
            package.tagCode[x] = nano.msg[31 + x];
      }
      
      if (!isTheSame(package, lastPackage)) {
        myTransfer.sendDatum(package.tagCode);

        for (byte x = 0 ; x < TAG_CODE_LENGTH ; x++) {
            lastPackage.tagCode[x] = package.tagCode[x];
      }
      }
      Serial.println("Previous TAG!");
        

      // Parameter for debug, set NANO_PARAMETER to value 1 to active
      #ifdef NANO_PARAMETER
            //If we have a full record we can pull out the fun bits
            int rssi = nano.getTagRSSI(); //Get the RSSI for this tag read
            long freq = nano.getTagFreq(); //Get the frequency this tag was detected at
            long timeStamp = nano.getTagTimestamp(); //Get the time this was read, (ms) since last keep-alive message
            byte tagEPCBytes = nano.getTagEPCBytes(); //Get the number of bytes of EPC from response
            Serial.print(F(" rssi["));
            Serial.print(rssi);
            Serial.print(F("]"));
      
            Serial.print(F(" freq["));
            Serial.print(freq);
            Serial.print(F("]"));
      
            Serial.print(F(" time["));
            Serial.print(timeStamp);
            Serial.print(F("]"));
      
            //Print EPC bytes, this is a subsection of bytes from the response/msg array and also is the only code needed to send
            Serial.print(F(" epc["));
            for (byte x = 0 ; x < tagEPCBytes ; x++)
            {
              if (nano.msg[31 + x] < 0x10) Serial.print(F("0")); //Pretty print
              Serial.print(nano.msg[31 + x], HEX);
              Serial.print(F(" "));
            }
            Serial.print(F("]"));
      
            Serial.println();
      #endif

    }

    // Check CRC error
    else if (responseType == ERROR_CORRUPT_RESPONSE) {
      Serial.println("Bad CRC");
    }
    else {
      Serial.print("Unknown error");
    }
  }

  if (isStopReading && detectTouch)
  {
    isStartReading = 1;
    isScanning = 0;
    isStopReading = 0;
    
    Serial.println("Start reading");
    NanoWakeUp();//nano.startReading(); //Begin scanning for tags
    //delay(2000);
  } 

  if (isStartReading && detectTouch)
  {
    isStartReading = 0;
    isScanning = 1;
    isStopReading = 0;
    
    Serial.println("scanning state");
  } 

  if (isScanning && detectTouch)
  {
    isStartReading = 0;
    isScanning = 1;
    isStopReading = 0;
    
    Serial.println("scanning state");
  } 

  delay(2000);
  if (isScanning && !detectTouch)
  {
    isStartReading = 0;
    isScanning = 0;
    isStopReading = 1;

    Serial.println("stop reading");
    NanoSleep();
    //delay(2000);
    Serial.println("stop reading");
  } 
  detectTouch = 0;
  delay(500);
//  if (Serial.available())
//  {
//    isStopReading = 1;
//    Serial.read(); //Throw away character
//    Serial.println("Paused");
//    NanoSleep();//nano.stopReading();
//    delay(2000);
//  }
}

//----------------------- SubFunction -----------------------//

void gotTouch(){
 Serial.println("Touch detected");
 detectTouch = true;
}

bool isTheSame(Package package1, Package package2) {
  for (int i = 0; i < TAG_CODE_LENGTH; i++){
    if (package1.tagCode[i] != package2.tagCode[i]) {
      return false;
    }
  }
  return true;
}

void NanoWakeUp()
{
  nano.begin(Serial2); 
  Serial2.begin(115200);
  //NanoSerial.begin(9600);       // Start serial to 9600 baud

  while (! nano.setPowerMode(0)) {
    delay(1500);                 // give time to wakeup
    Serial.println("retry wake up");
   //NanoSerial.begin(NANOSPEED); // Start the serial port, this time at user's chosen baud rate
  }

  nano.startReading();          // Begin scanning for tags
}

void NanoSleep()
{
    nano.stopReading();           // Stop scanning for tags

    while (nano.check());         // Try to flush as much as possible pending input
    while (Serial2.available()) Serial2.read();
    delay(1500);
    while (Serial2.available()) Serial2.read();
    while (nano.check());

    while (!nano.setPowerMode(3)); // set Maximum Saving mode
}