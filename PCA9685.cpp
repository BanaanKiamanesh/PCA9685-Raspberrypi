#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "PCA9685.h"

// Module Init
PCA9685::PCA9685(const int Addr)
{
    I2C_FD = wiringPiI2CSetup(Addr);
    if (I2C_FD == -1)
    {
        std::cout << "Device Not Found!";
        return;
    }

    setup();
    setPWMFreq(1000);
    delay(100);
}

// Get Dev Status Indicator
int PCA9685::getFD()
{
    return I2C_FD;
}

// Sets PCA9685 Module Mode to Normal Mode
void PCA9685::setup()
{
    wiringPiI2CWriteReg8(I2C_FD, MODE1, 0x00); // Normal mode
    wiringPiI2CWriteReg8(I2C_FD, MODE2, 0x04); // totem pole
}

// Set the PWM Frequency
void PCA9685::setPWMFreq(int freq)
{
    uint8_t prescale_val = (CLOCK_FREQ / 4096 / freq) - 1;
    wiringPiI2CWriteReg8(I2C_FD, MODE1, 0x10);             // Sleep
    wiringPiI2CWriteReg8(I2C_FD, PRE_SCALE, prescale_val); // Multiplyer for PWM Freq
    wiringPiI2CWriteReg8(I2C_FD, MODE1, 0x80);             // Restart
    wiringPiI2CWriteReg8(I2C_FD, MODE2, 0x04);             // Totem Pole (Default)
}

// Set PWM Signal for a Single Channel
void PCA9685::setPWM(uint8_t pin, int val)
{
    setPWM(pin, 0, val);
}

// Set PWM Signal for a Single Channel with Specific ON Time
void PCA9685::setPWM(uint8_t pin, int on_value, int off_value)
{
    wiringPiI2CWriteReg8(I2C_FD, PIN0_ON_L + PIN_MULTIPLYER * (pin - 1), on_value & 0xFF);
    wiringPiI2CWriteReg8(I2C_FD, PIN0_ON_H + PIN_MULTIPLYER * (pin - 1), on_value >> 8);
    wiringPiI2CWriteReg8(I2C_FD, PIN0_OFF_L + PIN_MULTIPLYER * (pin - 1), off_value & 0xFF);
    wiringPiI2CWriteReg8(I2C_FD, PIN0_OFF_H + PIN_MULTIPLYER * (pin - 1), off_value >> 8);
}

// Get current PWM value
int PCA9685::getPWM(uint8_t pin)
{
    int val = 0;
    val = wiringPiI2CReadReg8(I2C_FD, PIN0_OFF_H + PIN_MULTIPLYER * (pin - 1));
    val = val & 0xf;
    val <<= 8;
    val += wiringPiI2CReadReg8(I2C_FD, PIN0_OFF_L + PIN_MULTIPLYER * (pin - 1));
    return val;
}