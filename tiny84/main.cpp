#define timeLapseMinutes 3

#define errCount 8
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "dbg_putchar.h"

#include "pins.hh"

#include "rtc.h" 

#define t0presc0	1
#define t0presc8	2
#define t0presc64	3
#define t0presc256	4
#define t0presc1024	5

#define int0Low 0
#define int0Change 1
#define int0Fall 2
#define int0Rise 3

#define stateOff 0
#define stateVideo 1
#define stateFoto 2
#define stateMdVideo 3
#define stateMdFoto 4
#define stateUSB 5
#define stateError 6
#define stateRecording 7
#define stateTV 8
#define stateUnknown 9

/*
PA0 - play/exec button (OUT)
PA1 - Power button (OUT)
PA2 -
PA3 - Serial debug out
PA4 - I2C clock (ISP)
PA5 - (ISP)
PA6 - I2C data (ISP)
PA7 - BLUE led (IN)

PB0 - 
PB1 - Orange led (IN)
PB2 - RED led (IN)
PB3 - reset (ISP)
*/


DigitalOut<PortA::pin0> execPin;
DigitalOut<PortA::pin1> powerPin;

DigitalIn<PortA::pin7> redLed;
DigitalIn<PortB::pin8> blueLed;
DigitalIn<PortB::pin9> orangeLed;

volatile uint8_t cnt=0;
volatile uint8_t oldcnt=0;
volatile uint8_t blink=0;


uint8_t isMotion(){
    return orangeLed.read() ? 1 : 0;
}

uint8_t isFoto(){
    return redLed.read() ? 1 : 0;
}

uint8_t isVideo(){
    return blueLed.read() ? 1 : 0;
}


uint8_t bl=0;	

uint8_t cameraState(){

    
    if (isMotion() && isFoto()) return stateMdFoto;
    if (isMotion() && isVideo()) return stateMdVideo;
    if (isFoto() && isVideo()) return stateTV;
    if (isFoto()) return stateFoto;
    blink=0;
    _delay_ms(1500);
	bl=blink;
	
    
    //dbg_putchar('S');
    
    //dbg_putchar(bl);
	if ((bl>0x08) && (bl<0x18)) //error, blue fast blink
	{
        //dbg_putchar(bl);
		return stateError;
	}
	else if ((bl>0x38) && (bl<0x48)) //recording, blue slow blink
	{
		return stateRecording;
	}
    else {                
        if (isVideo()) return stateVideo;        
    }
	return stateOff;	
}


// delay 4500
void turnCameraOff(){
	powerPin.high();
	_delay_ms(4000);
	powerPin.low();
	_delay_ms(500);
}

// delay 2250
void turnOnCamera(){
	powerPin.high();
	_delay_ms(750);
	powerPin.low();
	_delay_ms(1500);
}

// delay 1500
void modeButton(){
	powerPin.high();
	_delay_ms(750);
	powerPin.low();
	_delay_ms(750);
}
// delay1500
void execButton(){
	execPin.high();
	_delay_ms(500);
	execPin.low();
	_delay_ms(500);
}

uint8_t errCnt=0;

int main()
{
	
	//dbg_tx_init();
	//dbg_putchar('\n');
    
	TCCR0B |=t0presc64;
	TIFR0 |= 0x01;
	TIMSK0 |= 0x01;
	
	MCUCR |=int0Change;
	GIMSK |=1<<6;
	
	sei();
	
	ADCSRA &= ~(1<<ADEN);	//disable ADC
    
    
    uint8_t a;
    errCnt=0;

    rtcInit();        
    turnOnCamera();
    _delay_ms(3000);
    blink=0;
    modeButton();
    
    _delay_ms(250); //compensation for precise RTC
    rtcSetAlarm(timeLapseMinutes);    	
    while(1){
		a=cameraState();
		//dbg_putchar(a+0x30);
        //dbg_putchar(errCnt+0x30);
		//dbg_putchar('\n');
		switch (a){
			case stateError:
                errCnt++;
                //dbg_putchar('e');
				break;		
			case stateRecording:
                //dbg_putchar('R');
                execButton();
				break;
			case stateOff:
                //dbg_putchar('o');
                turnOnCamera();
                break;
			case stateFoto:
                //dbg_putchar('f');
                errCnt++;
                _delay_ms(500);
                execButton();
                _delay_ms(3500);
                turnCameraOff();
                //dbg_putchar('\n');
                errCnt=9;
				break;
			case stateMdVideo:
                //dbg_putchar('V');                
                modeButton();
                errCnt++;
                if (errCnt>=errCount) break;
				break;
			case stateMdFoto:
                //dbg_putchar('F');
                modeButton();
                errCnt++;
                if (errCnt>=errCount) break;
				break;
			case stateVideo:
                //dbg_putchar('v');
                //while (isVideo()){
                    modeButton();
                    errCnt++;
                    if (errCnt>=errCount) break;
                //}
                break;
			default:
                //dbg_putchar('W');
				break;
		}
        if (errCnt>=errCount){
            rtcTurnOff();
        }
	}
	
}

ISR ( TIM0_OVF_vect ) { cnt++; }

ISR(EXT_INT0_vect){
	blink=cnt-oldcnt;
	oldcnt=cnt;
}
