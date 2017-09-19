#ifndef _LSM6DSL_H_
#define _LSM6DSL_H_

#include <stdbool.h>
#include <stdint.h>

//LSM6DSL - ACCELEROMETER & GRYOSCOPE
//REGISTERS
#define LSM6DSL_SENSOR_SYNC_TIME_FRAME	0x04
#define LSM6DSL_SENSOR_SYNC_RES_RATIO	0x05

#define LSM6DSL_FIFO_CTRL_1				0x06
#define LSM6DSL_FIFO_CTRL_2				0x07
#define LSM6DSL_FIFO_CTRL_3				0x08
#define LSM6DSL_FIFO_CTRL_4				0x09
#define LSM6DSL_FIFO_CTRL_5				0x0A

#define LSM6DSL_DRDY_PULSE_CFG_G		0x0B

#define LSM6DSL_INT1_CTRL				0x0D
#define LSM6DSL_INT2_CTRL				0x0E

#define LSM6DSL_WHO_AM_I				0x0F

#define LSM6DSL_CTRL1_XL				0x10
#define LSM6DSL_CTRL2_G					0x11
#define LSM6DSL_CTRL3_C					0x12
#define LSM6DSL_CTRL4_C					0x13
#define LSM6DSL_CTRL5_C					0x14
#define LSM6DSL_CTRL6_C					0x15
#define LSM6DSL_CTRL7_G					0x16
#define LSM6DSL_CTRL8_XL				0x17
#define LSM6DSL_CTRL9_XL				0x18
#define LSM6DSL_CTRL10_C				0x19

#define LSM6DSL_WAKEUP_SRC				0x1B
#define LSM6DSL_TAP_SRC					0x1C
#define LSM6DSL_D6D_SRC					0x1D
#define LSM6DSL_STATUS_REG				0x1E

#define LSM6DSL_OUT_TEMP_L				0X20
#define LSM6DSL_OUT_TEMP_H				0x21

#define LSM6DSL_OUTX_L_G				0x22
#define LSM6DSL_OUTX_H_G				0x23
#define LSM6DSL_OUTY_L_G				0x24
#define LSM6DSL_OUTY_H_G				0x25
#define LSM6DSL_OUTZ_L_G				0x26
#define LSM6DSL_OUTZ_H_G				0x27

#define LSM6DSL_OUTX_L_XL				0x28
#define LSM6DSL_OUTX_H_XL				0x29
#define LSM6DSL_OUTY_L_XL				0x2A
#define LSM6DSL_OUTY_H_XL				0x2B
#define LSM6DSL_OUTZ_L_XL				0x2C
#define LSM6DSL_OUTZ_H_XL				0x2D


#define LSM6DSL_FUNC_SRC1				0x53
#define LSM6DSL_TAP_CFG					0x58
#define LSM6DSL_TAP_THS_6D				0x59
#define LSM6DSL_WAKE_UP_THS				0x5B
#define LSM6DSL_WAKE_UP_DUR				0x5C
#define LSM6DSL_FREE_FALL				0x5D
#define LSM6DSL_MD1_CFG					0x5E

#define LSM6DSL_X_OFS_USR				0x73
#define LSM6DSL_Y_OFS_USR				0x74
#define LSM6DSL_Z_OFS_USR				0x75

//VALUES
#define LSM6DSL_WHO_AM_I_VALUE			0x6A

//LSM6DSL_CTRL3_C	PARAMETERS
#define LSM6DSL_SW_RESET				0x01

//LSM6DSL_TAP_CFG	PARAMETERS
#define LSM6DSL_INTERRUPTS_ENABLE		0x80
#define LSM6DSL_INACT_EN_3				0x60	//Gyro power down
#define LSM6DSL_INACT_EN_2				0x40	//Gyro sleep mode
#define LSM6DSL_INACT_EN_1				0x20	//Gyro does not change
#define LSM6DSL_SLOPE_FDS_HPF			0x10

#define LSM6DSL_TAP_XYZ_EN				0x0E
#define LSM6DSL_TAP_X_EN				0x08
#define LSM6DSL_TAP_Y_EN				0x04
#define LSM6DSL_TAP_Z_EN				0x02
#define LSM6DSL_LIR						0x01

//LSM6DSL_TAP_THS_6D	PARAMETERS

#define LSM6DSL_SIXD_THS_50_DEG			0x60
#define LSM6DSL_SIXD_THS_60_DEG			0x40
#define LSM6DSL_SIXD_THS_70_DEG			0x20

#define LSM6DSL_TAP_THS_MAX				0x1F
#define LSM6DSL_TAP_THS_90_PER			0x1C
#define LSM6DSL_TAP_THS_80_PER			0x19
#define LSM6DSL_TAP_THS_60_PER			0x13
#define LSM6DSL_TAP_THS_50_PER			0x0F
#define LSM6DSL_TAP_THS_40_PER			0x0C
#define LSM6DSL_TAP_THS_30_PER			0x09
#define LSM6DSL_TAP_THS_20_PER			0x06
#define LSM6DSL_TAP_THS_10_PER			0x03

//LSM6DSL_WAKE_UP_THS	PARAMETERS
#define LSM6DSL_WK_THS_MAX				0x3F	//1 LSb corresponds to FS_XL/26
#define LSM6DSL_WK_THS_50_PER			0x1F
#define LSM6DSL_WK_THS_10_PER			0x06
#define LSM6DSL_WK_THS_5_PER			0x03
#define LSM6DSL_WK_THS_1_PER			0x01

//LSM6DSL_WAKE_UP_DUR	PARAMETERS
#define LSM6DSL_WAKE_DUR_3				0x60	//3 ODR_time
#define LSM6DSL_WAKE_DUR_2				0x40	//2 ODR_time
#define LSM6DSL_WAKE_DUR_1				0x20	//1 ODR_time

//LSM6DSL_MD1_CFG	PARAMETERS

#define LSM6DSL_INT1_SINGLE_TAP			0x40	//Route Single Tap interrupt to INT1
#define LSM6DSL_INT1_WU					0x20	//Route wake up interrupt to INT1
#define LSM6DSL_INT1_FF					0x10	//Free fall interrupt to INT1
#define LSM6DSL_INT1_DOUBLE_TAP			0x08	//Route Double Tap interrupt to INT1
#define LSM6DSL_INT1_6D					0x04	//Route 6D interrupt to INT1
#define LSM6DSL_INT1_TILT				0x02	//Route tilt interrupt to INT1




//LSM6DSL_CTRL1_XL	PARAMETERS
#define LSM6DSL_ODR_XL_POWER_DOWN		0x00
#define LSM6DSL_ODR_XL_2HZ				0xB0
#define LSM6DSL_ODR_XL_12HZ				0x10
#define LSM6DSL_ODR_XL_26HZ				0x20
#define LSM6DSL_ODR_XL_52HZ				0x30
#define LSM6DSL_ODR_XL_104HZ			0x40
#define LSM6DSL_ODR_XL_208HZ			0x50
#define LSM6DSL_ODR_XL_416HZ			0x60
#define LSM6DSL_ODR_XL_833HZ			0x70
#define LSM6DSL_ODR_XL_1600HZ			0x80
#define LSM6DSL_ODR_XL_3330HZ			0x90
#define LSM6DSL_ODR_XL_6660HZ			0xA0

#define LSM6DSL_FS_XL_2G				0x00	//.061 mg/LSB	- Normal/Low power mode noise 1.8 mg - High power mode noise 90 ug/Hz
#define LSM6DSL_FS_XL_16G				0x04	//.488 mg/LSB	- Normal/Low power mode noise 3.0 mg - High power mode noise 130 ug/Hz
#define LSM6DSL_FS_XL_4G				0x08	//.122 mg/LSB	- Normal/Low power mode noise 2.0 mg - High power mode noise 90 ug/Hz
#define LSM6DSL_FS_XL_8G				0x0C	//.244 mg/LSB	- Normal/Low power mode noise 2.4 mg - High power mode noise 90 ug/Hz

#define LSM6DSL_LPF1_BW_SEL				0x02
#define LSM6DSL_BW0_XL_1500HZ			0x00
#define LSM6DSL_BW0_XL_400HZ			0x01

//LSM6DSL_CTRL2_G	PARAMETERS
#define LSM6DSL_ODR_G_POWER_DOWN		0x00
#define LSM6DSL_ODR_G_12HZ				0x10
#define LSM6DSL_ODR_G_26HZ				0x20
#define LSM6DSL_ODR_G_52HZ				0x30
#define LSM6DSL_ODR_G_104HZ				0x40
#define LSM6DSL_ODR_G_208HZ				0x50
#define LSM6DSL_ODR_G_416HZ				0x60
#define LSM6DSL_ODR_G_833HZ				0x70
#define LSM6DSL_ODR_G_1600HZ			0x80
#define LSM6DSL_ODR_G_3330HZ			0x90
#define LSM6DSL_ODR_G_6660HZ			0xA0

#define LSM6DSL_FS_125_125DPS			0x02	//4.375 mdps/LSB - Normal/Low power mode noise 75 mdps

#define LSM6DSL_FS_G_245DPS				0x00	//8.75 mdps/LSB
#define LSM6DSL_FS_G_500DPS				0x04	//17.5 mdps/LSB
#define LSM6DSL_FS_G_1000DPS			0x08	//35 mdps/LSB
#define LSM6DSL_FS_G_2000DPS			0x0C	//70 mdps/LSB	/ High power mode noise 4 mdps/Hz

//LSM6DSL_CTRL4_C	PARAMETERS
#define LSM6DSL_SLEEP					0x40
#define LSM6DSL_LPF1_SEL_G				0x02

//LSM6DSL_CTRL6_C	PARAMETERS
#define LSM6DSL_XL_HM_MODE_DISABLE		0x10
#define LSM6DSL_USR_OFF_W_2POW10_GLSB	0x00	//2^-10 g/LSB
#define LSM6DSL_USR_OFF_W_2POW6_GLSB	0x08	//2^-6 g/LSB
#define LSM6DSL_FTYPE_3					0x03
#define LSM6DSL_FTYPE_2					0x02
#define LSM6DSL_FTYPE_1					0x01
#define LSM6DSL_FTYPE_0					0x00

//LSM6DSL_CTRL7_G	PARAMETERS
#define LSM6DSL_G_HM_MODE_DISABLE		0x80
#define LSM6DSL_HPM_G_3					0x30
#define LSM6DSL_HPM_G_2					0x20
#define LSM6DSL_HPM_G_1					0x10
#define LSM6DSL_HPM_G_0					0x00

#define LSM6DSL_SERIAL_PORT_SPI			0x00
#define LSM6DSL_SERIAL_PORT_I2C			0x01

//LSM6DSL_CTRL8_XL	PARAMETERS
#define LSM6DSL_LPF2_XL_EN				0x80
#define LSM6DSL_HPCF_XL_3				0x60
#define LSM6DSL_HPCF_XL_2				0x40
#define LSM6DSL_HPCF_XL_1				0x20
#define LSM6DSL_HPCF_XL_0				0x00
#define LSM6DSL_INPUT_COMPOSITE			0x08

//LSM6DSL_CTRL10_C	PARAMETERS
#define LSM6DSL_WRIST_TILT_EN			0x80
#define LSM6DSL_PEDO_EN					0x10
#define LSM6DSL_TILT_EN					0x08
#define LSM6DSL_FUNC_EN					0x04
#define LSM6DSL_SIGN_MOTION_EN			0x01

bool initLsm6dsl(uint8_t , uint8_t );
bool sleepLsm6dsl(void);
bool sleep2Lsm6dsl(void);
bool sleep3Lsm6dsl(void);
bool wakeLsm6dsl(void);

bool getLsm6dslAccelData(uint8_t* );
bool getLsm6dslGyroData(uint8_t* );
bool getLsm6dslTempData(uint8_t*);

#endif
