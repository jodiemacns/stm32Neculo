/**
  ******************************************************************************
  * @file           : app_uart.c
  * @brief          : Main program body
  ******************************************************************************
  */

#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>
#include "app_uart.h"
#include "buffer.h"
#include "menu.h"

#define MAX_RX   1
#define TX_WAIT  1000

extern UART_HandleTypeDef huart1;

#define BUFFER_SIZE_UART (BUFFER_MEDIUM)

T_BUFFER bufferUartRx;
uint8_t  rxdata[BUFFER_SIZE_UART];

uint8_t aTxStartMessage[] = "\r\n-- APP UART app_uart 2: " __TIME__ "-- \r\n";
uint8_t aRxBuffer[MAX_RX * 2];
uint8_t gotSomething = 0;
uint8_t readDataBuffer[BUFFER_SIZE_UART];
static void appPrint(char *pData);
int toggleLED(unsigned char *pData, int length);
int printHelp(unsigned char *pData, int length);
void startMenu(void);

const MENU_TABLE commandTable[] = 
{ 
   { "?",     "Print help",     printHelp },
   { "toggle","Toggle the LED", toggleLED }
};       

/**
  * @brief app_uart will just do a simple test of the uart 
  * @retval None
  */
void app_uart(void)
{
   memset(&bufferUartRx, 0, sizeof(T_BUFFER));
   
   // Initialize the UART receive buffer
   bufferUartRx.size = BUFFER_SIZE_UART;
   bufferUartRx.pBuf = rxdata;
   initBuffer(&bufferUartRx);

   
   HAL_UART_Transmit(&huart1, (uint8_t *)aTxStartMessage, sizeof(aTxStartMessage), TX_WAIT);
   HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);
   HAL_Delay(100);
   appPrint("Hello there\n");
   HAL_Delay(1000);
   startMenu();
   
   while(1) {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      HAL_Delay(1000);
      if(gotSomething)
      {
         (void)readBuffer(&bufferUartRx, readDataBuffer, BUFFER_SIZE_UART);
         appPrint("GOT::");
         appPrint((char*)readDataBuffer);
         
//         HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);
//         HAL_UART_Transmit(&huart1, readDataBuffer, rlen, TX_WAIT);
         gotSomething = 0;
      }
   }
   return;
}

/**
  * @brief Rx Transfer completed callback
  * @param huart: UART handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

   /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file
   */
    uartReceive(*aRxBuffer);
    return;
}

void uartReceive(uint8_t rchar)
{
   T_BUFFER *pBuffer = &bufferUartRx;

   if(rchar == '\n' || rchar == '\r')
   {
      gotSomething = 1;
   }else
   {
      buff_write_char(pBuffer, rchar);
   }

   // Echo the character
   HAL_UART_Transmit(&huart1, &rchar, 1,TX_WAIT);
   HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, MAX_RX);

   return;
}

void startMenu(void)
{
   MENU newMenu;
   
   memset(&newMenu, 0, sizeof(MENU));
   
   newMenu.pPrint = appPrint;
   
   initialize_menu(&newMenu);
   print_menu(commandTable, (sizeof(commandTable) / sizeof(MENU_TABLE)));

}

static void appPrint(char *pData)
{
   size_t length = strlen(pData);

   for(int i = 0; i < length; i++) 
   {
      if(*pData == '\n')
      {
         char send = '\r';
         HAL_UART_Transmit(&huart1, (uint8_t *)&send, 1, TX_WAIT);
      }
      HAL_UART_Transmit(&huart1, (uint8_t *)pData++, 1, TX_WAIT);
   }
   return;
}


int toggleLED(unsigned char *pData, int length)
{
   return(0);
}

int printHelp(unsigned char *pData, int length)
{
   return(0);
}
