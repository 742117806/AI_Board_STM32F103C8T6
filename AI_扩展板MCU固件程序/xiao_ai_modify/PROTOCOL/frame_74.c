

#include "includes.h"
/****************************************************************
**��   �ܣ�����Ӧ��Э�����74����(��һ֡���ݵ��û���������ʼ��֡������ǰ�������)
**��   ����
        @srcData Դ����
        @srcLen Դ���ݵĳ��ȣ��ֽ�����
		@outLen	�����һ֡���ݵĳ���
        @mode 1���룬0����
**����ֵ:��
****************************************************************/
void FrameData_74Convert(uint8_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode)
{
  uint8_t frame_len;
  uint8_t temp[256] = {0};
  uint16_t crc16;
  uint8_t *p_frame_data;

  if (mode == 0) //����
  {
    frame_len = _74DecodeBytes(&srcData[Region_DataAFNNumber], temp, srcLen - 11); //�ѱ������ӵ�CRC16(2���ֽ�)ȥ��
    temp[frame_len] = HKFreamEnd;
    memcpy(&srcData[Region_DataAFNNumber], temp, frame_len + 1);
    *outLen = frame_len + 9;
  }
  else //����
  {
    frame_len = _74CodeBytes(&srcData[Region_DataAFNNumber], temp, srcLen - 9); //74����

    memcpy(&srcData[Region_DataAFNNumber], temp, frame_len); //�ѱ���õ����ݸ��ƻ�ԭ�����ݵĻ�������
    crc16 = CRC16_2(srcData, frame_len + 8);     //����󳤶�+Э��֡ǰ��û�����8���ֽ�֡����
    p_frame_data = &srcData[Region_DataAFNNumber];
    p_frame_data[frame_len] = (crc16 >> 8);
    p_frame_data[frame_len + 1] = (crc16 & 0x00ff);
    p_frame_data[frame_len + 2] = HKFreamEnd; //��������֡����0x53
    *outLen = frame_len + 8 + 3;
  }
}

/****************************************************************
**��   �ܣ�����Ӧ��Э�����74����(��һ֡���ݵ��û���������ʼ��֡������ǰ�������)
**��   ����
        @srcData Դ����
        @srcLen Դ���ݵĳ��ȣ��ֽ�����
		@outLen	�����һ֡���ݵĳ���
        @mode 1���룬0����
**����ֵ:��
****************************************************************/
void FrameRouteData_74Convert(uint8_t *srcData, uint8_t srcLen, uint8_t *outLen, uint8_t mode)
{
  static uint8_t frame_len;
  uint8_t temp[256] = {0};
  uint16_t crc16;
  uint8_t *p_frame_data;
	uint8_t _74ConvertLen = 0;

  if (mode == 0) //����
  {
		_74ConvertLen =  (srcLen - 9);
		frame_len = _74DecodeBytes(&srcData[5], temp, _74ConvertLen); //�ѱ������ӵ�CRC16(2���ֽ�)ȥ��
		srcData[2] = frame_len +3;
		srcData[3] = ~srcData[2];
		temp[frame_len] = 0x96;
		temp[frame_len+1] = 0x96;
		memcpy(&srcData[5], temp, frame_len+2);


    
    *outLen = srcData[2] + 4;
  }
  else //����
  {
		_74ConvertLen = srcLen - 7;
    frame_len = _74CodeBytes(&srcData[5], temp, _74ConvertLen); //74����

    memcpy(&srcData[5], temp, frame_len); //�ѱ���õ����ݸ��ƻ�ԭ�����ݵĻ�������
		srcData[2] = frame_len+5;
		srcData[3] = ~srcData[2]; 
    crc16 = CRC16_2((uint8_t *)srcData, frame_len + 5);     //����󳤶�+Э��֡ǰ��û�����8���ֽ�֡����
    p_frame_data = srcData;
    p_frame_data[srcData[2]] = (crc16 >> 8);
    p_frame_data[srcData[2] + 1] = (crc16 & 0x00ff);
    p_frame_data[srcData[2] + 2] = 0x96; //��������֡����0x53
	p_frame_data[srcData[2] + 3] = 0x96; //��������֡����0x53
    *outLen = srcData[2] + 4;
  }
}
