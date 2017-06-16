/*
LoraRemote - Receiver side
Simple, Lora based, long range remote control switch
Perfect for using the Adafruit Feather M0 Lora Board
*/

#include <SPI.h>
#include <RH_RF95.h>
#include <RTCZero.h>

/* for feather m0 */ 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

RTCZero rtc;

// Change to the desired Frequency in respect of the frequency range of the used module
#define RF95_FREQ 464.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

#define LED 13
#define OUT 12 // you can chose the desired Output pin to use here

void setup() 
{
  pinMode(LED, OUTPUT); 
  pinMode(OUT, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  digitalWrite(LED, LOW);

    //RTC
  rtc.begin();
  rtc.setTime(0,0,0);
  rtc.setDate(1,1,2017);
    
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
    0x00  // Reg 0x26: LowDataRate=OFF, Agc=OFF
  };
  rf95.setModemRegisters(&modem_config);
  rf95.spiWrite(RH_RF95_REG_0C_LNA, 0x23);//LNA to MAX
  rf95.setPreambleLength(250);  
  rf95.setTxPower(23);
}

/* Do a power Nap -> powers everything down for x seconds to save a lot of power*/
void powerNap(int seconds){
  if(seconds > 60)
    seconds = 60;
  rtc.setTime(0,0,0);
  rtc.setAlarmTime(0, 0, seconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(ISR_Alarm);
  rtc.standbyMode();
}

void ISR_Alarm(){
  rtc.disableAlarm();
}

void cadWaitLoop(){
    while(!rf95.isChannelActive()){
      powerNap(2); //if no Channel Activity was detected, go take a nap for 2 Seconds!
      // These 2 Seconds are kind of synchronized with the set preamble length, so that there is a high possibility to detect a Channel Activity if the transmitter is on
    }
    //if a Channel Activity is detected, drop out of the function here
}

void loop()
{
  
  cadWaitLoop(); // wait for Channe Activity whith sleeping alot
  rf95.waitAvailableTimeout(4000); //wait a long enough time for receiving the Packet
  // Should be a message for us now   
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf); 
  if (rf95.recv(buf, &len))
  {
    //Compare the received message: if it is "A1!", switch on the Output, if it is "A0!", switch off the Output
    switch(buf[0]){
      case 'A':
        if(buf[1] == '1' && buf[2] == '!' && len == 4){
          //enable out
           digitalWrite(LED, HIGH);
           digitalWrite(OUT, HIGH);
           //Uncomment and edit the lines below, if you want, that the Output automatically switches off after a certain time, time is set in the delay(x) function in Milliseconds: 5000 = 5 Seconds
//           delay(5000);
//           digitalWrite(LED, LOW);
//           digitalWrite(OUT, LOW);
          }
        else if(buf[1] == '0' && buf[2] == '!'  && len == 4){
            //disable out
            digitalWrite(LED, LOW);
            digitalWrite(OUT, LOW);
          }
        break;
    }
  }
}
