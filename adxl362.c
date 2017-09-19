#include "adxl362.h"
#include "spi.h"
#include "usart.h"
#include "delay.h"

static uint8_t serialType = ADXL362_SERIAL_PORT_SPI;
static uint8_t adxl362SpiCsPin = 0;
static uint8_t initializedFlag = 0;

bool initAdxl362(uint8_t type, uint8_t spiCsPin) {	// init ADXL362 sensor using SPI interface
	uint8_t tx_data[3];
	uint8_t rx_data[3];
	bool errorFlag = 0;

	uint32_t temp = 0 ;

	serialType = type;
	adxl362SpiCsPin = spiCsPin;

	if (serialType == ADXL362_SERIAL_PORT_SPI) {
		
		//Soft Reset
		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_SOFT_RESET;
		tx_data[2] = ADXL362_SOFT_RESET_VALUE;
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_THRESH_ACT_L\n", 0);
		}
		delay_ms(5);	//Required a 0.5ms delay

		//setup interrupt mode
		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_THRESH_ACT_L;
		//tx_data[2] = 0xFA; //256 mg
		tx_data[2] = 0x70; //256 mg
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_THRESH_ACT_L\n", 0);		
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_THRESH_ACT_H;
		tx_data[2] = 0x01; //256mg*H + L
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_THRESH_ACT_H\n", 0);
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_THRESH_INACT_L;
		tx_data[2] = 0x00; //256 mg
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_THRESH_INACT_L\n", 0);
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_THRESH_INACT_H;
		tx_data[2] = 0x05; //256mg*H + L 
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_THRESH_INACT_H\n", 0);
		}


		//TIME**********************************************************************

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_TIME_ACT;
		tx_data[2] = 0x05; //Active for 5 samples
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_TIME_ACT\n", 0);
		}
		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_TIME_INACT_L;
		//tx_data[2] = 0x1E; //Inactive for 30 samples
		tx_data[2] = 0x01;
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_TIME_INACT_L\n", 0);
		}
		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_TIME_INACT_H;
		tx_data[2] = 0x00; //256*H + L samples
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_TIME_INACT_L\n", 0);
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_ACT_INACT_CTL;
		tx_data[2] = (ADXL362_LOOP_MODE | ADXL362_INACT_REF | ADXL362_INACT_EN | ADXL362_ACT_REF | ADXL362_ACT_EN);
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_ACT_INACT_CTL\n", 0);
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_INTMAP2;
		//tx_data[2] = ADXL362_INTMAP2_AWAKE;
		tx_data[2] = (ADXL362_INTMAP2_AWAKE | ADXL362_INTMAP2_ACT_INT);		
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_INTMAP2\n", 0);
		}

		tx_data[0] = ADXL362_WRITE_REGISTER;
		tx_data[1] = ADXL362_POWER_CTL;
		//tx_data[2] = (ADXL362_POWER_CTL_WAKEUP | ADXL362_POWER_CTL_MEASUREMENT_MODE);
		tx_data[2] = (ADXL362_POWER_CTL_WAKEUP | ADXL362_POWER_CTL_AUTOSLEEP | ADXL362_POWER_CTL_MEASUREMENT_MODE);		
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_POWER_CTL\n", 0);
		}

		tx_data[0] = ADXL362_READ_REGISTER;
		tx_data[1] = ADXL362_DEVID_AD;
		tx_data[2] = 0x00;
		if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
			errorFlag = 1;
			printUSART0("ADXL362_DEVID_AD\n", 0);
		}
		if (rx_data[2] != ADXL362_DEVID_AD_VALUE) {
			errorFlag = 1;

			temp = rx_data[2];
			printUSART0("ADXL362_DEVID_AD FAIL: [%h]\n", &temp);
		}
	}
	else {
		errorFlag = 1;
	}

	if (errorFlag == 0) {
		initializedFlag = 1;
	}

	delay_ms(100);
	return errorFlag;
}
bool sleepAdxl362(void) {
	uint8_t tx_data[3];
	uint8_t rx_data[3];
	bool errorFlag = 0;

	if (initializedFlag) {
		if (serialType == ADXL362_SERIAL_PORT_SPI) {
			tx_data[0] = ADXL362_WRITE_REGISTER;
			tx_data[1] = ADXL362_POWER_CTL;
			tx_data[2] = 0x00;
			if (rxtxSPI0(3, tx_data, rx_data, adxl362SpiCsPin)) {
				errorFlag = 1;
			}
		}
	}
	return errorFlag;
}