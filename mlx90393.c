#include "mlx90393.h"
#include "spi.h"
#include "usart.h"
#include "delay.h"

static uint8_t serialType = MLX90393_SERIAL_PORT_SPI;
static uint8_t mlx90393SpiCsPin = 0;
static bool initializedFlag = 0;

static uint8_t initialStateData[6];

bool initMlx90393(uint8_t type, uint8_t spiCsPin) {
	uint8_t tx_data[8];
	uint8_t rx_data[8];
	uint16_t commandValue;
	bool errorFlag = 0;

	uint32_t temp = 0;

	serialType = type;
	mlx90393SpiCsPin = spiCsPin;

	if (serialType == MLX90393_SERIAL_PORT_SPI) {
		//Must perform exit mode command after reset

		//Reset
		tx_data[0] = MLX90393_RESET;		//Command
		if (rxtxSPI0(1, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_RESET\n", 0);
		}
		
		delay_ms(5);

		//Exit Mode
		tx_data[0] = MLX90393_EXIT_MODE;		//Command
		tx_data[1] = 0x00;
		if (rxtxSPI0(2, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_EXIT_MODE\n", 0);
		}
		//temp = rx_data[1];
		//printUSART0("Exit Mode - Staus Byte: [%h]\n", &temp);

		//Test Write IC
		tx_data[0] = MLX90393_WRITE_REGISTER | 0x0F;		//Command
		commandValue = MLX90393_TEST_VALUE;
		tx_data[1] = (commandValue >> 8);
		tx_data[2] = (commandValue & 0x00FF);
		tx_data[3] = (MLX90393_VOL_MEM_REG_0A << 2);
		tx_data[4] = 0x00;
		if (rxtxSPI0(5, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_VOL_MEM_REG_0A\n", 0);
		}
		//temp = rx_data[4];
		//printUSART0("Test Write Register - Status Byte: [%h]\n", &temp);

		//Test Read IC
		tx_data[0] = MLX90393_READ_REGISTER;		//Command
		tx_data[1] = (MLX90393_VOL_MEM_REG_0A << 2);
		tx_data[2] = 0x00;
		tx_data[3] = 0x00;
		tx_data[4] = 0x00;

		if (rxtxSPI0(5, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_VOL_MEM_REG_0A-2\n", 0);
		}
		/*
		if (rx_data[3] != (MLX90393_TEST_VALUE >> 8) && rx_data[4] != (MLX90393_TEST_VALUE & 0x00FF)) {
			errorFlag = 1;

			temp = rx_data[2];
			printUSART0("Read Register - Status Byte: [%h]\n", &temp);
			temp = rx_data[3];
			printUSART0("Read Register - Data H: [%h]\n", &temp);
			temp = rx_data[4];
			printUSART0("Read Register - Data L: [%h]\n", &temp);
		}
		*/

		//Setup Z GAIN HALL & Bitvalue 0.979 uT/LSB
		tx_data[0] = MLX90393_WRITE_REGISTER;		//Command
		//commandValue = (MLX90393_GAIN_SEL_7 | MLX90393_HALL_CONF_SPIN_3);	//Set to 4.5Hz DIG_FILT=7 OSR=3
		commandValue = (MLX90393_GAIN_SEL_5);	
		tx_data[1] = (commandValue >> 8);
		tx_data[2] = (commandValue & 0x00FF);
		tx_data[3] = (MLX90393_Z_GAIN_HALL << 2);
		tx_data[4] = 0x00;
		if (rxtxSPI0(5, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_OSR_RES_DIG\n", 0);
		}
		//temp = rx_data[4];
		//printUSART0("Z GAIN HALL - Staus Byte: [%h]\n", &temp);

		//Setup Data Rate
		tx_data[0] = MLX90393_WRITE_REGISTER;		//Command
		commandValue = (MLX90393_TRIG_INT_DISABLE | MLX90393_COMM_MODE_SPI | MLX90393_BURST_RATE_1);		//Set to 5*20ms BURSTRATE=5
		tx_data[1] = (commandValue >> 8);
		tx_data[2] = (commandValue & 0x00FF);
		tx_data[3] = (MLX90393_TRIG_COMM_WOC_TCMP_BURST << 2);
		tx_data[4] = 0x00;
		if (rxtxSPI0(5, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_TRIG_COMM_WOC_TCMP_BURST\n", 0);
		}
		//temp = rx_data[4];
		//printUSART0("Data Rate - Status Byte: [%h]\n", &temp);

		//Setup Sampling Rate
		tx_data[0] = MLX90393_WRITE_REGISTER;		//Command
		//commandValue = (MLX90393_RES_XYZ_3 | MLX90393_DIG_FILT_7 | MLX90393_OSR_3);
		commandValue = (MLX90393_RES_XYZ_1 | MLX90393_DIG_FILT_3 | MLX90393_OSR_1);	//Set to 4.53ms OSR=1 DIG_FILT=3
		tx_data[1] = (commandValue >> 8);
		tx_data[2] = (commandValue & 0x00FF);
		tx_data[3] = (MLX90393_OSR_RES_DIG << 2);
		tx_data[4] = 0x00;
		if (rxtxSPI0(5, tx_data, rx_data, mlx90393SpiCsPin)) {
			errorFlag = 1;
			printUSART0("MLX90393_OSR_RES_DIG\n", 0);
		}
		//temp = rx_data[4];
		//printUSART0("Sample Rate - Staus Byte: [%h]\n", &temp);

	}
	else {
		errorFlag = 1;
	}

	if (errorFlag == 0) {
		initializedFlag = 1;
	}

	return errorFlag;
}
bool sleepMlx90393(void) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];
	bool errorFlag = 1;

	if (initializedFlag) {
		if (serialType == MLX90393_SERIAL_PORT_SPI) {
			//Reset
			tx_data[0] = MLX90393_RESET;		//Command
			if (rxtxSPI0(1, tx_data, rx_data, mlx90393SpiCsPin)) {
				errorFlag = 1;
				printUSART0("MLX90393_RESET\n", 0);
			}
		}
	}
	return errorFlag;
}
bool wakeMlx90393(void) {
	uint8_t tx_data[1];
	uint8_t rx_data[1];
	bool errorFlag = 1;
	
	if (initializedFlag) {
		if (serialType == MLX90393_SERIAL_PORT_SPI) {
			//Start Single Mode
			tx_data[0] = (MLX90393_START_BURST_MODE | MLX90393_SELECT_ZYX);		//Command
			if (rxtxSPI0(1, tx_data, rx_data, mlx90393SpiCsPin) == 0) {
				errorFlag = 0;
			}
		}
	}
	return errorFlag;
}

bool getMlx90393MagData(uint8_t* magDataPtr) {
	uint8_t tx_data[8] = { 0 };
	uint8_t rx_data[8] = { 0 };
	uint32_t timer = 0;
	bool errorFlag = 1;

	if (initializedFlag) {
		if (serialType == MLX90393_SERIAL_PORT_SPI) {

			//Start Single Mode
			tx_data[0] = (MLX90393_START_SINGLE_MEASUREMENT_MODE | MLX90393_SELECT_ZYX);		//Command
			tx_data[1] = 0x00;
			rxtxSPI0(2, tx_data, rx_data, mlx90393SpiCsPin);

			//Francesco review timer delay
			while (!nrf_gpio_pin_read(MLX90393_INT_PIN) && timer<1000) {
				timer++;
			}
			
			tx_data[0] = (MLX90393_READ_MEASUREMENT | MLX90393_SELECT_ZYX);	//Command
			if (rxtxSPI0(8, tx_data, rx_data, mlx90393SpiCsPin) == 0) {
				errorFlag = 0;

				for (int i = 0; i < 6; i++) {
						magDataPtr[i] = rx_data[i + 2];
				}
			}
		}
	}
	return errorFlag;
}

