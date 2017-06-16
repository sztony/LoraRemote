/*
LoraRemote - Transmitter side
Simple, Lora based, long range remote control switch
Perfect for using the Adafruit Feather M0 Lora Board
*/
 
#include <SPI.h>
#include <RH_RF95.h>

/* for feather m0 */ 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to the desired init Frequency in respect of the frequency range of the used module
#define RF95_FREQ 464.0
// you can define many different frequencys for multiple receivers (should be a few Mhz apart from each other)
// must match with the one set on the receiver
#define RECEIVER1 464.0
#define RECEIVER2 484.0
 
RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED 13

// connect pushbuttons on the chosen pins; pushbutton should connect to ground level
#define RECEIVER1_ON 6 
#define RECEIVER1_OFF 7
#define RECEIVER2_ON 8
#define RECEIVER2_OFF 9
#define LEDSEND 5 //connect an LED for transmitter activity display

float curFreq = RF95_FREQ;
 
void setup() 
{
  pinMode(LED, OUTPUT); 
  digitalWrite(LED, LOW);


  pinMode(LEDSEND, OUTPUT); 
  pinMode(RECEIVER1_ON, INPUT_PULLUP); 
  pinMode(RECEIVER1_OFF, INPUT_PULLUP); 
  pinMode(RECEIVER2_ON, INPUT_PULLUP); 
  pinMode(RECEIVER2_OFF, INPUT_PULLUP); 

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    while (1);
  }
 
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }

  // Modem Config chosen for low data rate but long range
  RH_RF95::ModemConfig modem_config = {
    0x64, // Reg 0x1D: BW=62,5kHz, Coding=4/6, Header=explicit
    0x94, // Reg 0x1E: Spread=512chips/symbol-> SF = 9, CRC=enable
    0x00  // Reg 0x26: LowDataRate=OFF, Agc=OFF : 0x00
  };
  rf95.setModemRegisters(&modem_config);
  rf95.spiWrite(RH_RF95_REG_0C_LNA, 0x23);//LNA to MAX
  rf95.setPreambleLength(250);  
  rf95.setTxPower(23);
}

bool setFreq(float freq){
  if(curFreq == freq) return true;
  if (!rf95.setFrequency(freq)) {
    return false;
  }
  delay(100);
  curFreq = freq;
  return true;
}

void setOutput(float freq, bool out){
  digitalWrite(LEDSEND, HIGH);
  setFreq(freq);
  char data[] = "A0!";
  if(out)
    data[1] = '1';
  else
    data[1] = '0';
  rf95.send((uint8_t *)data, sizeof(data));
  rf95.waitPacketSent();
  digitalWrite(LEDSEND, LOW);
}
  
void loop()
{
  //define the desired button functions here, note: if a button is pushed, the digitalRead value is 0, else it is 1
  //first receiver ON/OFF
  if(digitalRead(RECEIVER1_ON) == 0)
    setOutput(RECEIVER1, 1);
  else if(digitalRead(RECEIVER1_OFF) == 0)
    setOutput(RECEIVER1, 0);

  //second receiver ON/OFF
  else if(digitalRead(RECEIVER2_ON) == 0)
    setOutput(RECEIVER2, 1);
  else if(digitalRead(RECEIVER2_OFF) == 0)
    setOutput(RECEIVER2, 0);
  else
    delay(10);
}
    
