#include "rtc.h"

uint8_t initSeconds;

uint8_t rtcReadByte(uint8_t regAddress){
  uint8_t c=0;
  TinyWireM.beginTransmission(rtcAddress); // transmit to device
  TinyWireM.write(uint8_t(regAddress));            // sends instruction byte
  TinyWireM.endTransmission();     // stop transmitting
  TinyWireM.requestFrom(rtcAddress, 1);    // request 1 byte from slave device 
  while (TinyWireM.available()) { // slave may send less than requested
    c = TinyWireM.read(); // receive a byte as character
  }
  return c;
  
}

void rtcWriteByte(uint8_t regAddress, uint8_t data){
  TinyWireM.beginTransmission(rtcAddress); // transmit to device
  TinyWireM.write(regAddress);            // sends instruction byte
  TinyWireM.write(data);
  TinyWireM.endTransmission();     // stop transmitting
  
}

void rtcSetAlarm(uint8_t minutes){
    rtcWriteByte(0x0,7);
    
    for (uint8_t i=1;i<0x8;i++){
        rtcWriteByte(i,0);
    }
    
    rtcWriteByte(0x8,minutes);
    rtcWriteByte(0x9,0b10000000);
    rtcWriteByte(0xa,0b10000000);
    rtcWriteByte(0xb,minutes);
    rtcWriteByte(0xc,0b10000000);
    rtcWriteByte(0xd,0b10000000);
}

uint8_t bin2bcd(uint8_t v){
   return ((v / 10)<<4) + (v % 10);
}

void stayOn(void){
    rtcSetAlarm(1);
    rtcWriteByte(0x0,bin2bcd(59));
    //_delay_ms(1100);
}
void setIRQAlarm(){
  rtcWriteByte(rtcControl,0b00000111);
}

uint8_t rtcGetStatus(){
  return  rtcReadByte(rtcStatus); // receive a byte
}

uint8_t getControl(){
  return rtcReadByte(rtcControl); // receive a byte
}

void rtcTurnOff(){
    rtcWriteByte(rtcStatus,0);
}

void rtcInit(){
  TinyWireM.begin();
  setIRQAlarm();
  stayOn();
  //setAlarm(minutes);
  //resetAlarm();
}
