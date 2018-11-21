#include "stm32f0_usart.h"
#include "uart.h"

UpCom_Rx_TypDef UpCom_RxBuf;
//UartFrame_TX_TypDef     UpCom_TxBuf;

uint8_t Secret_GKey_Flag;  //密钥公钥存在标志
uint8_t Secret_KeyOk_Flag; //密钥OK标志

Uart_ErrorType UpUart_DataTx(uint8_t *p_buf, uint8_t len, UartFrame_TX_TypDef *pu_buf)
{
  UNUSED(pu_buf);
  UartSendBytes(USART1, p_buf, len);
  return Uart_NoError;
}

uint8_t UpUart_TX_INT_Process(UartFrame_TX_TypDef *pu_buf)
{
  UNUSED(pu_buf);
  return 0;
}
