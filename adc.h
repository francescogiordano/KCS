#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

void InitAdc(uint8_t );
void EnableAdc(void);
void DisableAdc(void);
uint16_t GetAdcValue(void);

#endif
