#ifndef __USART_H
#define __USART_H

#include "nrf.h"
#include "nrf_gpio.h"
#include "gpio.h"
#include "misc.h"

#define USART0_TX				UART_TX_PIN
#define USART0_RX				UART_RX_PIN


#define USAR0_BAUDRATE_9600					0x00275000	
#define USAR0_BAUDRATE_115200				0x01D7E000
#define USAR0_BAUDRATE_230400				0x03AFB000	
#define USAR0_BAUDRATE_460800				0x075F7000	
#define USAR0_BAUDRATE_921600				0x0EBEDFA4			

void initUSART0(uint8_t tx, uint8_t rx, uint32_t baudrate);
void sleepUSART0(void);

void putCharUSART0(uint8_t data);
uint8_t readCharUSART0(void);
void printUSART0(char* str, uint32_t* num);
 
#endif 
