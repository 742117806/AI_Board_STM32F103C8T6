#include "deviceInfo.h"
#include "string.h"
#include "wireless_app.h"
#include "delay.h"

//设备信息
DeviceInfo_t  deviceInfo;

OldDevice_t 	oldDevice = {
    .num = 0,
    .buff = {0},
};

uint8_t lowPowerDevice[100]={0};		//低功耗设备

void LowPowerDeviceInit(void)
{
	STMFLASH_Read(LOW_PWR_DEVICE_ADDR,(uint16_t*)lowPowerDevice,50);
	if((lowPowerDevice[0]==0xff)&&(lowPowerDevice[1]==0xff))
	{
		memset(lowPowerDevice,0x00,100);
		STMFLASH_Write(LOW_PWR_DEVICE_ADDR,(uint16_t*)lowPowerDevice,50);
	}
}


uint8_t IsLowPowerDevice(uint8_t mac_bit7)
{
   	uint8_t lowPowerflag = 0;
	
	lowPowerflag = mac_bit7&0x30;
	lowPowerflag >>= 4;
	if(lowPowerflag == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//检测低功耗设备是否已经存在了
uint8_t LowPowerDeviceIsExsit(uint8_t addr)
{
	uint8_t i=0;
	for(i=0;i<100;i++)
	{
		if(lowPowerDevice[i] == addr)
		{
			return 1;
		}			
	}
	return 0;
}
uint8_t  LowPowerDeviceInset(uint8_t mac_bit7,uint8_t addr)
{
	uint8_t i=0;
	uint8_t lowPowerflag = 0;
	
	lowPowerflag = mac_bit7&0x30;
	lowPowerflag >>= 4;
	
	if(lowPowerflag == 1)
	{
		if(LowPowerDeviceIsExsit(addr) == 1)return 1;
		for(i=0;i<100;i++)
		{
			if(lowPowerDevice[i] == 0)
			{
				lowPowerDevice[i] =  addr;
				STMFLASH_Write(LOW_PWR_DEVICE_ADDR,(uint16_t*)lowPowerDevice,50);
				return 1;
			}
		}
	}
	return 0;
}


void LowPowerDeviceDelete(void)
{
	uint8_t i=0;
	uint8_t j = 0;
	

	for(j=0;j<100;j++)
	{
		for(i=0;i<deviceInfo.deviceNum;i++)
		{
			if(lowPowerDevice[j] == deviceInfo.deviceBuff[i])
			{
				break;
			}
		}
		if(i ==deviceInfo.deviceNum)
		{
			lowPowerDevice[j] = 0; 
		}
	}
	STMFLASH_Write(LOW_PWR_DEVICE_ADDR,(uint16_t*)lowPowerDevice,50);

}

uint8_t  LowPowerDeviceMach(uint8_t addr)
{
  	uint8_t i=0;
	for(i=0;i<100;i++)
	{
		if(lowPowerDevice[i] == addr)
		{	
			return 1;
		}
	}
	return 0;
}


void LowPowerDeviceWakeUp(uint8_t ch)
{
	uint8_t i = 0;
	for(i=0;i<17;i++)
	{
		Si4438_Transmit_Start(&Wireless_Buf,ch,(uint8_t*)"1", 1);
		//while(WIRELESS_STATUS != Wireless_TX_Finish);
		delay_ms(25);
	}
}




