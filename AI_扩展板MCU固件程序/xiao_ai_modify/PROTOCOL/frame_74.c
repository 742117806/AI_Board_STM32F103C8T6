

#include "includes.h"
/****************************************************************
**功   能：按照应用协议进行74编码(从一帧数据的用户数据区开始到帧结束符前面的数据)
**参   数：
        @srcData 源数据
        @srcLen 源数据的长度（字节数）
		@outLen	编码后一帧数据的长度
        @mode 1编码，0解码
**返回值:无
****************************************************************/
void FrameData_74Convert(uint8_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode)
{
  uint8_t frame_len;
  uint8_t temp[256] = {0};
  uint16_t crc16;
  uint8_t *p_frame_data;

  if (mode == 0) //解码
  {
    frame_len = _74DecodeBytes(&srcData[Region_DataAFNNumber], temp, srcLen - 11); //把编码过后加的CRC16(2个字节)去掉
    temp[frame_len] = HKFreamEnd;
    memcpy(&srcData[Region_DataAFNNumber], temp, frame_len + 1);
    *outLen = frame_len + 9;
  }
  else //编码
  {
    frame_len = _74CodeBytes(&srcData[Region_DataAFNNumber], temp, srcLen - 9); //74编码

    memcpy(&srcData[Region_DataAFNNumber], temp, frame_len); //把编码好的数据复制回原来数据的缓存区，
    crc16 = CRC16_2(srcData, frame_len + 8);     //编码后长度+协议帧前面没编码的8个字节帧数据
    p_frame_data = &srcData[Region_DataAFNNumber];
    p_frame_data[frame_len] = (crc16 >> 8);
    p_frame_data[frame_len + 1] = (crc16 & 0x00ff);
    p_frame_data[frame_len + 2] = HKFreamEnd; //编码后加上帧结束0x53
    *outLen = frame_len + 8 + 3;
  }
}

/****************************************************************
**功   能：按照应用协议进行74编码(从一帧数据的用户数据区开始到帧结束符前面的数据)
**参   数：
        @srcData 源数据
        @srcLen 源数据的长度（字节数）
		@outLen	编码后一帧数据的长度
        @mode 1编码，0解码
**返回值:无
****************************************************************/
void FrameRouteData_74Convert(uint8_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode)
{
  static uint8_t frame_len;
  uint8_t temp[256] = {0};
  uint16_t crc16;
  uint8_t *p_frame_data;
	uint8_t _74ConvertLen = 0;

  if (mode == 0) //解码
  {
		_74ConvertLen =  (srcLen - 9);
		frame_len = _74DecodeBytes(&srcData[5], temp, _74ConvertLen); //把编码过后加的CRC16(2个字节)去掉
		srcData[2] = frame_len +3;
		srcData[3] = ~srcData[2];
		temp[frame_len] = 0x96;
		temp[frame_len+1] = 0x96;
		memcpy(&srcData[5], temp, frame_len+2);


    
    *outLen = srcData[2] + 4;
  }
  else //编码
  {
		_74ConvertLen = srcLen - 7;
    frame_len = _74CodeBytes(&srcData[5], temp, _74ConvertLen); //74编码

    memcpy(&srcData[5], temp, frame_len); //把编码好的数据复制回原来数据的缓存区，
		srcData[2] = frame_len+5;
		srcData[3] = ~srcData[2]; 
    crc16 = CRC16_2((uint8_t *)srcData, frame_len + 5);     //编码后长度+协议帧前面没编码的8个字节帧数据
    p_frame_data = srcData;
    p_frame_data[srcData[2]] = (crc16 >> 8);
    p_frame_data[srcData[2] + 1] = (crc16 & 0x00ff);
    p_frame_data[srcData[2] + 2] = 0x96; //编码后加上帧结束0x53
	p_frame_data[srcData[2] + 3] = 0x96; //编码后加上帧结束0x53
    *outLen = srcData[2] + 4;
  }
}
