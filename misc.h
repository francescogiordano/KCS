#ifndef __MISC_H
#define __MISC_H

#include "nrf.h"

#define MAX_PRINT_STRING_SIZE		128

void getDNum(uint8_t * dnum, uint32_t num);
void putDNum(uint8_t * dnum, uint16_t * m, uint8_t * r_str);
void getASCIIString(char * str, uint32_t  * num, uint8_t * r_str);

void catstr(char* str1, char* str2);
#endif 
