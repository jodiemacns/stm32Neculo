#include <stdio.h>
#include <string.h>
#include "menu.h"

static void pad_string(int used_len, int padding, char pad_char);

MENU menu;

int initialize_menu(MENU *p_menu)
{
   memcpy(&menu, p_menu, sizeof(MENU));
   menu.pPrint("Initialized menu\n");
   return(0);
}

void pad_string(int used_len, int padding, char pad_char)
{
   char string[] = " ";

   string[0] = pad_char;
   
   for(int i = 0;  i < (padding - used_len); i++)
   {
      menu.pPrint(string);
   }
   return;
}

void print_menu(const MENU_TABLE *pCommands, int num_commands)
{
   for(int count = 0; count < num_commands; count++)
   {
      if(pCommands[count].pCmd)
      {
         menu.pPrint(pCommands[count].pCmd);
         pad_string(strlen(pCommands[count].pCmd), 30, ' ');
         menu.pPrint("   :   ");
         menu.pPrint(pCommands[count].pDescription);
         menu.pPrint("\n");
      }
   }
   return;
}

void run_command(const MENU_TABLE *pCommands, int num_commands, char *pString)
{
   for(int count = 0; count < num_commands; count++)
   {
      if(strncmp(pCommands[count].pCmd, pString, strlen(pCommands[count].pCmd)))
      {
         (pCommands[count].mcb)(NULL, 0);
      }
   }
   return;
}
