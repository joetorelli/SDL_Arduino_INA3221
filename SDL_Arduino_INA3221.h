//
//   SDL_Arduino_INA3221 Library
//   SDL_Arduino_INA3221.cpp Arduino code - runs in continuous mode
//   Version 1.2
//   SwitchDoc Labs   September 2019
//
//  added three channels
//  looking into adding on chip averaging

/**************************************************************************/
/*!
    Initial code from INA219 code (Basically just a core structure left)
    @author   K. Townsend (Adafruit Industries)
    @license  BSD (see BSDlicense.txt)

    */
/**************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define INA3221_ADDRESS (0x40) // 1000000 (A0+A1=GND)
#define INA3221_READ (0x01)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER (R/W)
    -----------------------------------------------------------------------*/
#define INA3221_REG_CONFIG (0x00)
/*---------------------------------------------------------------------*/
#define INA3221_CONFIG_RESET (0x8000) // Reset Bit

#define INA3221_CONFIG_ENABLE_CHAN1 (0x4000) // Enable Channel 1
#define INA3221_CONFIG_ENABLE_CHAN2 (0x2000) // Enable Channel 2
#define INA3221_CONFIG_ENABLE_CHAN3 (0x1000) // Enable Channel 3

#define INA3221_CONFIG_AVG2 (0x0800) // AVG Samples Bit 2 - See table 3 spec
#define INA3221_CONFIG_AVG1 (0x0400) // AVG Samples Bit 1 - See table 3 spec
#define INA3221_CONFIG_AVG0 (0x0200) // AVG Samples Bit 0 - See table 3 spec

#define INA3221_CONFIG_VBUS_CT2 (0x0100) // VBUS bit 2 Conversion time - See table 4 spec
#define INA3221_CONFIG_VBUS_CT1 (0x0080) // VBUS bit 1 Conversion time - See table 4 spec
#define INA3221_CONFIG_VBUS_CT0 (0x0040) // VBUS bit 0 Conversion time - See table 4 spec

#define INA3221_CONFIG_VSH_CT2 (0x0020) // Vshunt bit 2 Conversion time - See table 5 spec
#define INA3221_CONFIG_VSH_CT1 (0x0010) // Vshunt bit 1 Conversion time - See table 5 spec
#define INA3221_CONFIG_VSH_CT0 (0x0008) // Vshunt bit 0 Conversion time - See table 5 spec

#define INA3221_CONFIG_MODE_2 (0x0004) // Operating Mode bit 2 - See table 6 spec
#define INA3221_CONFIG_MODE_1 (0x0002) // Operating Mode bit 1 - See table 6 spec
#define INA3221_CONFIG_MODE_0 (0x0001) // Operating Mode bit 0 - See table 6 spec

/*=========================================================================*/

/*=========================================================================
    SHUNT VOLTAGE REGISTER (R)
    -----------------------------------------------------------------------*/
#define INA3221_REG_SHUNTVOLTAGE_1 (0x01)
/*=========================================================================*/

/*=========================================================================
    BUS VOLTAGE REGISTER (R)
    -----------------------------------------------------------------------*/
#define INA3221_REG_BUSVOLTAGE_1 (0x02)
/*=========================================================================*/

#define SHUNT_RESISTOR_VALUE (0.1) // default shunt resistor value of 0.1 Ohm

// Averaging modes
typedef enum
{
    INA3221_REG_CONF_AVG_1 = 0,
    INA3221_REG_CONF_AVG_4,
    INA3221_REG_CONF_AVG_16,
    INA3221_REG_CONF_AVG_64,
    INA3221_REG_CONF_AVG_128,
    INA3221_REG_CONF_AVG_256,
    INA3221_REG_CONF_AVG_512,
    INA3221_REG_CONF_AVG_1024
} ina3221_avg_mode_t;

class SDL_Arduino_INA3221
{

    // Configuration register
    typedef struct
    {
        uint16_t mode_shunt_en : 1;
        uint16_t mode_bus_en : 1;
        uint16_t mode_continious_en : 1;
        uint16_t shunt_conv_time : 3;
        uint16_t bus_conv_time : 3;
        uint16_t avg_mode : 3;
        uint16_t ch3_en : 1;
        uint16_t ch2_en : 1;
        uint16_t ch1_en : 1;
        uint16_t reset : 1;
    } conf_reg_t; //__attribute__((packed));

public:
    SDL_Arduino_INA3221(uint8_t addr = INA3221_ADDRESS, float shuntresistor_1 = SHUNT_RESISTOR_VALUE, float shuntresistor_2 = SHUNT_RESISTOR_VALUE, float shuntresistor_3 = SHUNT_RESISTOR_VALUE);
    void begin(void);
    float getBusVoltage_V(int channel);
    float getShuntVoltage_mV(int channel);
    float getCurrent_mA(int channel);
    int getManufID();

    // Sets shunt resistor value in mOhm
    void setShuntRes(float res_ch1, float res_ch2, float res_ch3);

    // Sets filter resistors value in Ohm
    void setFilterRes(uint32_t res_ch1, uint32_t res_ch2, uint32_t res_ch3);

    uint8_t INA3221_i2caddr;
    float INA3221_shuntresistor[3];

    void wireWriteRegister(uint8_t reg, uint16_t *value);
    void wireReadRegister(uint8_t reg, uint16_t *value);
    void INA3221SetConfig(void);
    int16_t getBusVoltage_raw(int channel);
    int16_t getShuntVoltage_raw(int channel);
    // Sets averaging mode. Sets number of samples that are collected
    // and averaged togehter.
    void setAveragingMode(ina3221_avg_mode_t mode);
};
