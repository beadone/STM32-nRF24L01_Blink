/*

 Sensor Receiver.
 Each sensor modue has a unique ID.
 
 TMRh20 2014 - Updates to the library allow sleeping both in TX and RX modes:
      TX Mode: The radio can be powered down (.9uA current) and the Arduino slept using the watchdog timer
      RX Mode: The radio can be left in standby mode (22uA current) and the Arduino slept using an interrupt pin
 */
 /* nrf24 pins
 * |v+  |gnd | 
 * |csn |ce  |
 * |Mosi|sck |
 * |irq |miso|
 * stm32 mapping
 * v+  -> 3.3v
 * grn -> grd
 * csn -> PB1
 * ce  -> PB0
 * IRQ -> PB10
 * sck -> PA5
 * MISO-> PA6
 * MOSI-> PA7
 * 
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Set up nRF24L01 radio on SPI-1 bus (MOSI-PA7, MISO-PA6, SCLK-PA5) ... IRQ not used?
RF24 radio(PB0,PB1);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0e2LL };   // Radio pipe addresses for the 2 nodes to communicate.
//const uint8_t addresses[][6] = {"1Node","2Node"};








//turn_on_led

char   turn_on_led[] = "on";
char   turn_off_led[] = "off";

uint8_t data[32];     //signal
int msglen;            // length of the message


void setup(){
     Serial.begin(115200);
  delay(1000);

   pinMode(PC13, OUTPUT);
  Serial.println("\n\rRF24 Sensor Receiver");
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  // Setup and configure rf radio

  radio.begin();


  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setChannel(0x4c);  //channel 76
  //radio.setPALevel(RF24_PA_LOW);   //RF_SETUP        = 0x03
  radio.setPALevel(RF24_PA_MAX);   //RF_SETUP        = 0x07
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  
//delay(10000);
// wait for the serial port

  // Open pipes to other nodes for communication

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

//  radio.openWritingPipe(addresses[0]); // transmitt
//  radio.openReadingPipe(1,addresses[1]);

  radio.openWritingPipe(pipes[0]); // transmitt
  radio.openReadingPipe(1,pipes[1]);




  // Start listening
  radio.startListening();

  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
   digitalWrite(PC13, LOW);
}


void loop(){
  
    byte pipeNo;
    //byte gotByte;                                       // Dump the payloads until we've gotten everything
    while( radio.available(&pipeNo)){
      //radio.read( &gotByte, 1 );
        msglen = radio.getDynamicPayloadSize();
        radio.read( &data, msglen );
        radio.writeAckPayload(pipeNo,&data, msglen );
      
        //radio.writeAckPayload(pipeNo,&gotByte, 1 );
    if (strncmp ((const char*)data,turn_on_led,2) == 0)
    {
          digitalWrite(PC13, LOW);
    }
    
    if (strncmp ((const char*)data,turn_off_led,3) == 0)
    {
          digitalWrite(PC13, HIGH);
    }



          
   }
 

}// main loop


