/*
 * lm73_regs.h
 *
 *  Created on: 2021. nov. 17.
 *      Author: livanyi
 */

#ifndef LM73_REGS_H_
#define LM73_REGS_H_

// Config reg bits
#define LM73_BIT_ONE_SHOT       0x04

// Control/status reg bits
#define LM73_BIT_ALRT_STAT      0x08
#define LM73_BIT_THI_FLAG       0x04
#define LM73_BIT_TLOW_FLAG      0x02
#define LM73_BIT_DAV_FLAG       0x01

//#define LM73_BIT_STATUS           0x0F

// Registers
#define LM73_REG_TEMPERATURE    0x00
#define LM73_REG_CONFIG         0x01
#define LM73_REG_THI            0x02
#define LM73_REG_TLOW           0x03
#define LM73_REG_CTRLSTATUS     0x04
#define LM73_REG_ID             0x07

// Register masks
// Config
#define LM73_MASK_PD            ~(LM73_POWER_OFF | LM73_POWER_ON)

// Control/status
#define LM73_MASK_RESOLUTION    ~(LM73_RESOLUTION_11BIT | LM73_RESOLUTION_12BIT | LM73_RESOLUTION_13BIT | LM73_RESOLUTION_14BIT)


typedef enum
{
    // LM73-0
    LM73_0_I2C_FLOAT = 0x48, // Float
    LM73_0_I2C_GND = 0x49, // Ground
    LM73_0_I2C_VDD = 0x4A, // VDD

    // LM73-1
    LM73_1_I2C_FLOAT = 0x4C, // Float
    LM73_1_I2C_GND = 0x4D, // Ground
    LM73_1_I2C_VDD = 0x4E, // VDD
} lm73_i2caddr_t;

typedef enum
{
    LM73_RESOLUTION_11BIT = 0b00000000,
    LM73_RESOLUTION_12BIT = 0b00100000,
    LM73_RESOLUTION_13BIT = 0b01000000,
    LM73_RESOLUTION_14BIT = 0b01100000
} lm73_resolution_t;

typedef enum
{
    LM73_POWER_ON = 0b00000000,
    LM73_POWER_OFF = 0b10000000,
} lm73_power_t;


#endif /* LM73_REGS_H_ */
