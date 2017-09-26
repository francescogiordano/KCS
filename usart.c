#include "usart.h"

void initUSART0(uint8_t tx, uint8_t rx, uint32_t baudrate) {
	// USART0 initialization on tx -> TX & rx -> RX
	NRF_GPIO->PIN_CNF[tx] = 0x00000001;									// set as output
	NRF_GPIO->PIN_CNF[rx] = 0x00000000;									// set as input, sense off, drive S0S1, pull-down
																		// connect input buffer  
	NRF_UART0->PSELTXD = tx;
	NRF_UART0->PSELRXD = rx;

	NRF_UART0->BAUDRATE         = baudrate;
	NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
	NRF_UART0->TASKS_STARTTX    = 1;
	NRF_UART0->TASKS_STARTRX    = 1;
	NRF_UART0->EVENTS_RXDRDY    = 0;
}
void putCharUSART0(uint8_t data) {
	NRF_UART0->TXD = data;
	while (NRF_UART0->EVENTS_TXDRDY != 1);
	NRF_UART0->EVENTS_TXDRDY = 0;
}
uint8_t readCharUSART0(void) {
	uint8_t data = 0;
	uint32_t timer = 0;

	while (NRF_UART0->EVENTS_RXDRDY != 1 && timer < 1000) {
		timer++;
	}

	if (NRF_UART0->EVENTS_RXDRDY) {
		NRF_UART0->EVENTS_RXDRDY = 0;
		data = NRF_UART0->RXD;
	}	
	return data;
}
void printUSART0(char* str, uint32_t* num) {		
	// print text and one signed integer or float number

	uint8_t r_str[MAX_PRINT_STRING_SIZE];
	uint16_t k = 0;

	getASCIIString(str, num, r_str);
	
	while(r_str[k] != '\0')
	{
		putCharUSART0(r_str[k]);
		if(r_str[k] == '\n')
			putCharUSART0('\r');
		k++;
		
		if(k == MAX_PRINT_STRING_SIZE)
			break;
	}
}
void sleepUSART0() {
	nrf_gpio_cfg_output(USART0_TX);
	nrf_gpio_pin_clear(USART0_TX);

	nrf_gpio_cfg_output(USART0_RX);
	nrf_gpio_pin_clear(USART0_RX);
}
