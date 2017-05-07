#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

#include <util/delay.h>

#include "TinyWireM.h"

#define onCompensation 7
#define rtcAddress          0B1101000

//#define rtcAlarm1           0x07
//#define rtcAlarm2           0x0B
//#define rtcAxMx             0B10000000

#define rtcControl          0x0E
#define rtcControl_EOSC     0B10000000
#define rtcControl_RS2      0B00010000
#define rtcControl_RS1      0B00001000
#define rtcControl_INTCN    0B00000100
#define rtcControl_A2IE     0B00000010
#define rtcControl_A1IE     0B00000001

#define rtcStatus           0x0F
#define rtcStatus_OSF       0B10000000
#define rtcStatus_A2F       0B00000010
#define rtcStatus_A1F       0B00000001


void stayOn(void);
void rtcTurnOff(void);
//uint8_t rtcGetStatus(void);

void rtcInit(void);
void rtcSetAlarm(uint8_t minutes);
