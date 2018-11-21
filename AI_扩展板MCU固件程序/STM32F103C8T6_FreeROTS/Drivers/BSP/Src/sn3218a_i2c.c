/**************************************************************************************
 * SN3218A LED  呼吸灯驱动
 * 模拟I2C驱动
 *
 *
 * ************************************************************************************/

#include "stm32f1xx.h"
#include "sn3218a_i2c.h"
#include "delay.h"

void I2c_Init(void)
{
}

//I2C开始
void I2C_Start(void)
{

    I2C_SDA_H();
    I2C_SCL_H();
    delay_us(1);
    //__NOP;
    I2C_SDA_L();
    delay_us(1);
    I2C_SCL_L();
}

//I2C停止
void I2C_Stop(void)
{
    I2C_SDA_L();
    I2C_SCL_H();
    delay_us(1);
    //__NOP;
    I2C_SDA_H();
    delay_us(1);
    I2C_SCL_L();
}

//I2C写数据
void I2C_Write1Byte(uint8_t buf)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        I2C_SCL_L();
        if (buf & 0x80)
            I2C_SDA_H();
        else
            I2C_SDA_L();
        delay_us(1);
        I2C_SCL_H();
        delay_us(1);
        buf <<= 1;
    }
    I2C_SCL_L();
    delay_us(1);
    I2C_SDA_H();
    delay_us(1);
    I2C_SCL_H();
    delay_us(1);

    while (I2C_SDA_RD() && (i < 255))
        i++;
    //while(SDA_RD(port));
    I2C_SCL_L();
    delay_us(1);
}
