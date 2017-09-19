#include "wdt.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

void initWDT(void) {
	NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);
	NRF_WDT->CRV = 10 * 32768;   //ca 3 sek. timout
	NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  //Enable reload register 0
}
void startWDT(void) {
	NRF_WDT->TASKS_START = 1;
}
void stopWDT(void) {
	NRF_WDT->TASKS_START = 0;
}
void tickleWDT(void) {
	NRF_WDT->RR[0] = WDT_RR_RR_Reload;  //Reload watchdog register 0
}