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

//------------------------------------------------------------------------------
// Macros
#define MAX_RX           1
#define TX_WAIT          1000
#define BUFFER_SIZE_UART (BUFFER_MEDIUM)

typedef enum
{
   LED_ON,
   LED_OFF,
   LED_FLASH
}LED_STATE;

//------------------------------------------------------------------------------
// Module Variables
static T_BUFFER bufferUartRx;             ///< Used for receive buffering.
static uint8_t rxdata[BUFFER_SIZE_UART];  ///< Used for holding for ::T_BUFFER.
static uint8_t isr_RxBuffer[MAX_RX * 2];  ///< Used to receive from the hal.
static uint8_t gotSomething = 0;          ///< Indicates that we received full byte.
static uint8_t readDataBuffer[BUFFER_SIZE_UART]; ///< Used for reading the bytes once received.
static LED_STATE ledState = LED_FLASH;

//------------------------------------------------------------------------------
// Functions 
static void appPrint(char *pData);
static int toggleLED(unsigned char *pData, int length);
static int printHelp(unsigned char *pData, int length);
static void startMenu(void);

///! Command table used to display the help menu
static const   MENU_TABLE commandTable[] = 
{ 
   { "?",     "Print help",                          printHelp },
   { "led",   "Toggle the [LED / ON / OFF / FLASH]", toggleLED }
};       

#define CMD_TBL_SIZE (sizeof(commandTable) / sizeof(MENU_TABLE))

//------------------------------------------------------------------------------
// External dependancies 
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef  htim3;

//------------------------------------------------------------------------------
/**
  * @brief app_uart will just do a simple test of the uart 
  * @retval None
  */
//------------------------------------------------------------------------------
void app_uart(void)
{
   // Initialize the UART receive buffer
   memset(&bufferUartRx, 0, sizeof(T_BUFFER));
   bufferUartRx.size = BUFFER_SIZE_UART;
   bufferUartRx.pBuf = rxdata;
   initBuffer(&bufferUartRx);

   // display the header message 
   appPrint("\n-------------------------------------------------\n");
   appPrint("-- APP UART app_uart 3:\n");
   appPrint("-------------------------------------------------\n");

   HAL_UART_Receive_IT(&huart1, (uint8_t *)isr_RxBuffer, MAX_RX);
   startMenu();

   // Start the timer;
   HAL_TIM_Base_Start_IT(&htim3);
   
   while(1) {
      HAL_Delay(10);
      if(gotSomething)
      {
         int rVal;
         memset(readDataBuffer, 0, BUFFER_SIZE_UART);
         (void)readBuffer(&bufferUartRx, readDataBuffer, BUFFER_SIZE_UART);
         rVal = run_command(commandTable, CMD_TBL_SIZE, (char*)readDataBuffer);
         if(rVal < 0)
         {
            appPrint("Command not found: ");
            appPrint((char*)readDataBuffer);
            appPrint("\n");
         }
         gotSomething = 0;
      }
   }
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
   HAL_UART_Receive_IT(&huart1, (uint8_t *)isr_RxBuffer, MAX_RX);

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
   appPrint("Todo Toggle the LED\n");

   if(!strncmp((char *)pData, "led ON", strlen("led ON")))
   {
      appPrint("::ON\n");
      ledState = LED_ON;
   }
   if(!strncmp((char *)pData, "led OFF", strlen("led OFF")))
   {
      ledState = LED_OFF;
      appPrint("::OFF\n");
   }
   if(!strncmp((char *)pData, "led FLASH", strlen("led FLASH")))
   {
      ledState = LED_FLASH;
      appPrint("::FLASH\n");
   }
   return(0);
}

int printHelp(unsigned char *pData, int length)
{
   print_menu(commandTable, (sizeof(commandTable) / sizeof(MENU_TABLE)));
   return(0);
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
    uartReceive(*isr_RxBuffer);
    return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   /* Prevent unused argument(s) compilation warning */
   UNUSED(htim);
   /* NOTE : This function Should not be modified, when the callback is needed,
            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
   */
   if(ledState == LED_ON)
   {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
   }
   if(ledState == LED_OFF)
   {
      HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
   }
   if(ledState == LED_FLASH)
   {
       HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
   }
   return;
}
