#include "sensors.h"
#include "ble_phy.h"
#include "usart.h"

static volatile uint8_t sleepSensorErrorFlag = 0;

uint8_t initSensors(void) {
	uint8_t errorFlag = 0;

	/*
	if (initAdxl362(ADXL362_SERIAL_PORT_SPI, ADXL362_SPI_CS)) {		// init ADXL362 sensor - Sleep Accel
		printUSART0("ADXL362 init     [FAIL]\n", 0);
		CharDiagInfoData[0] = 0x01;
		errorFlag = 1;
	}
	else {
		printUSART0("ADXL362 init     [OK]\n", 0);
		CharDiagInfoData[0] = 0x02;
	}
	*/
	if (initLsm6dsl(LSM6DSL_SERIAL_PORT_SPI, LSM6DSL_SPI_CS)) {		// init LSM6DSL sensor - Gyro & Accel
		printUSART0("U4 LSM6DSL [FAIL]\n", 0);
		CharDiagInfoData[0] = 0x00;
		errorFlag = 1;
		sleepSensorErrorFlag = 1;
	}
	else {
		printUSART0("U4 LSM6DSL [OK]\n", 0);
		CharDiagInfoData[0] = 0x01;
	}
	if (initMlx90393(MLX90393_SERIAL_PORT_SPI, MLX90393_SPI_CS)) {	// init MLX90393 sensor - Mag
		printUSART0("U3 MLX90393 [FAIL]\n", 0);
		CharDiagInfoData[1] = 0x00;
		errorFlag = 1;
	}
	else {
		printUSART0("U3 MLX90393 [OK]\n", 0);
		CharDiagInfoData[1] = 0x01;
	}
	return errorFlag;
}

double toDouble(uint8_t high, uint8_t low) {
	uint16_t conversion_form;

	conversion_form = (high << 8 | low);
	short shortVal = (short) conversion_form;
	if (shortVal<0) {
		return (double) (shortVal*-1);
	}
	return (double) shortVal;
}
double pythagore3(double a, double b, double c) {
	return sqrt((a*a) + (b*b) + (c*c));
}

void getDataSENSOR(uint8_t* sensorData) {
	uint8_t	tempData[6] = { 0 };
	uint16_t tempConversion;
	uint16_t conversion_form;
	static uint8_t counter;

	sensorData[0] = 0;
	sensorData[1] = 0;
	sensorData[2] = 0;
	sensorData[3] = 0;

	sensorData[4] = 0;
	sensorData[5] = 0;
	sensorData[6] = 0;
	sensorData[7] = 0;
	sensorData[8] = counter++;

	sensorData[9] = 0;
	
	/*
	sensorData[10] = 0;
	sensorData[11] = 0;
	sensorData[12] = 0;
	sensorData[13] = 0;
	sensorData[14] = 0;
	*/
	if (getMagData(tempData) == 0) {

		/*
		sensorData[9] = tempData[0];		//X_H
		sensorData[10] = tempData[1];		//X_L
		sensorData[11] = tempData[2];		//Y_H
		sensorData[12] = tempData[3];		//Y_L
		sensorData[13] = tempData[4];		//Z_H
		sensorData[14] = tempData[5];		//Z_L
		*/

		/*
		uint32_t temp;
		temp = tempData[0];
		printUSART0("XH: [%h]\n", &temp);
		temp = tempData[1];
		printUSART0("XL: [%h]\n", &temp);

		temp = tempData[2];
		printUSART0("YH: [%h]\n", &temp);
		temp = tempData[3];
		printUSART0("YL: [%h]\n", &temp);

		temp = tempData[4];
		printUSART0("ZH: [%h]\n", &temp);
		temp = tempData[5];
		printUSART0("ZL: [%h]\n", &temp);
		*/

		if ((tempData[0] & 0x80) == 0x80) {
			tempConversion = tempData[0] * 256 + tempData[1];
			tempConversion = (tempConversion ^ 0xFFFF) + 1;
			tempData[0] = tempConversion >> 8;
			tempData[1] = tempConversion & 0x00FF;
		}

		if ((tempData[2] & 0x80) == 0x80) {
			tempConversion = tempData[2] * 256 + tempData[3];
			tempConversion = (tempConversion ^ 0xFFFF) + 1;
			tempData[2] = tempConversion >> 8;
			tempData[3] = tempConversion & 0x00FF;
		}

		if ((tempData[4] & 0x80) == 0x80) {
			tempConversion = tempData[4] * 256 + tempData[5];
			tempConversion = (tempConversion ^ 0xFFFF) + 1;
			tempData[4] = tempConversion >> 8;
			tempData[5] = tempConversion & 0x00FF;
		}

		/*
		temp = tempData[0];
		printUSART0("+XH: [%h]\n", &temp);
		temp = tempData[1];
		printUSART0("+XL: [%h]\n", &temp);

		temp = tempData[2];
		printUSART0("+YH: [%h]\n", &temp);
		temp = tempData[3];
		printUSART0("+YL: [%h]\n", &temp);

		temp = tempData[4];
		printUSART0("+ZH: [%h]\n", &temp);
		temp = tempData[5];
		printUSART0("+ZL: [%h]\n", &temp);
		*/

		//conversion_form = pythagore3(toDouble(tempData[0], tempData[1]) / 16, toDouble(tempData[2], tempData[3]) / 16, toDouble(tempData[4], tempData[5]) / 16);
		conversion_form = pythagore3(toDouble(tempData[0], tempData[1]), toDouble(tempData[2], tempData[3]), toDouble(tempData[4], tempData[5]));

		/*
		temp = conversion_form;
		printUSART0("conversion_form: [%h]\n", &temp);
		*/

		sensorData[0] = conversion_form & 0xFF;			//Conversion Mag L
		sensorData[1] = conversion_form >> 8 & 0xFF;		//Conversion Mag H
	}
	if (getAccelData(tempData) == 0) {
		sensorData[2] = tempData[2];		//tempData[2] = Y_L
		sensorData[3] = tempData[3];		//tempData[3] = Y_H

		sensorData[4] = tempData[0];		//tempData[0] = X_L
		sensorData[5] = tempData[1];		//tempData[1] = X_H

											//tempData[4,5] = Z_L,Z_H

		//uint32_t temp = tempData[3]*256 + tempData[2];
		//printUSART0("%d\n", &temp);
	}
	if (getGyroData(tempData) == 0) {
		/*
		conversion_form = (uint16_t)toDouble(tempData[4], tempData[5]);	//tempData[4,5] = Z_L, Z_H

		sensorData[6] = conversion_form & 0xFF;
		sensorData[7] = (conversion_form >> 8);
		*/
		sensorData[6] = tempData[4];		//GYRO Z_L  
		sensorData[7] = tempData[5];		//GYRO Z_H

		//uint32_t temp = tempData[5] * 256 + tempData[4];
		//printUSART0("%d\n", &temp);
	}
}

bool getAccelData(uint8_t* accelDataPtr) {
	bool errorFlag = 1;

	errorFlag = getLsm6dslAccelData(accelDataPtr);

	return errorFlag;
}
bool getGyroData(uint8_t* gyroDataPtr) {
	bool errorFlag = 1;

	errorFlag = getLsm6dslGyroData(gyroDataPtr);

	return errorFlag;
}
bool getMagData(uint8_t* magDataPtr) {
	bool errorFlag = 1;

	errorFlag = getMlx90393MagData(magDataPtr);

	return errorFlag;
}
bool getTempData(uint8_t* tempDataPtr) {
	bool errorFlag = 1;

	errorFlag = getLsm6dslTempData(tempDataPtr);	//tempDataPtr[0] = TEMP_L, tempDataPtr[1] = TEMP_H

	return errorFlag;
}

uint8_t getSleepSensorErrorFlag(void) {
	return sleepSensorErrorFlag;
}

void initTIMER2(void) {
	
	// setup and start 16bit TIMER2 to measure elapsed time in ms and also trigger interrupt every ms for sensor data collection
	
	NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
	NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;			// Set counter to 16 bit resolution
	//NRF_TIMER2->PRESCALER = 0x0004;		// 16MHz clock & 16 prescaler means 1MHz timer clock
	NRF_TIMER2->PRESCALER = 0x0008;		// 16MHz clock & 256 prescaler means 62.5KHz timer clock

	NRF_TIMER2->TASKS_CLEAR = 1;        // clear the task first to be usable for later

	NRF_TIMER2->CC[0] = 6250;			//16-bit timer 6250 = 100ms @ 62.5Khz clock
	
	NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);
	
	NVIC_SetPriority(TIMER2_IRQn, 3);
	NVIC_EnableIRQ(TIMER2_IRQn);
}
void startTIMER2(void) {
	NRF_TIMER2->TASKS_START = 1;	// start TIMER2
}
void stopTIMER2(void) {
	NRF_TIMER2->TASKS_STOP = 1;		// stop TIMER2
}
void TIMER2_IRQHandler(void) {		
	//TIMER2 will decide how often to wak-up from powerManage()
	NRF_TIMER2->EVENTS_COMPARE[0] = 0;		// clear the interrupt flag	
	NRF_TIMER2->TASKS_CLEAR = 1;			// restart TIMER2
}
