/**
  ******************************************************************************
  * @file           : test.c
  * @brief          : Main program body
  ******************************************************************************
  */

#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>
#include "app_uart.h"
#include "buffer.h"

#define MAX_RX 1

extern UART_HandleTypeDef huart1;

#define BUFFER_SIZE_UART (BUFFER_REALLY_SMALL)

T_BUFFER bufferUartRx;
uint8_t  rxdata[BUFFER_SIZE_UART];

uint8_t aTxStartMessage[] = "\r\n-- APP UART test 2: " __TIME__ "-- \r\n";
uint8_t aRxBuffer[MAX_RX * 2];
uint8_t gotSomething = 0;
uint8_t readDataBuffer[BUFFER_SIZE_UART];


void test(void)
{
   uint8_t tmp;

   memset(&bufferUartRx, 0, sizeof(T_BUFFER));

   // Initialize the UART receive buffer
   bufferUartRx.size = BUFFER_SIZE_UART;
   bufferUartRx.pBuf = rxdata;
   initBuffer(&bufferUartRx);

   HAL_UART_Transmit_IT(&huart1, (uint8_t *)aTxStartMessage, sizeof(aTxStartMessage));
   HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);

   while(1) {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(1000);
    if(gotSomething)
    {
       int rlen;
       rlen = readBuffer(&bufferUartRx, readDataBuffer, BUFFER_SIZE_UART);
       tmp = '*';
       HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);
       HAL_UART_Transmit(&huart1, readDataBuffer, rlen, 0xFFFF);
       gotSomething = 0;
    }
   }
}

/**
  * @brief Rx Transfer completed callback
  * @param huart: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t err_msg[] = "\r\nFull Buffer!!!\r\n";
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file
   */

    HAL_UART_Transmit(&huart1, (uint8_t *)err_msg, (uint16_t)strlen((const char *)err_msg),0xFFFF);
    uartReceive(*aRxBuffer);
}

void uartReceive(uint8_t rchar)
{
   T_BUFFER *pBuffer = &bufferUartRx;

   if(rchar == '\n' || rchar == '\r')
   {
      rchar = ':';
      gotSomething = 1;
   }else
   {
      b_write(pBuffer, rchar);
   }

   HAL_UART_Transmit(&huart1, &rchar, 1,0xFFFF);
   HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);


   return;
}
