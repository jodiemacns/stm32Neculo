typedef int (*menu_callback)(unsigned char *pData, int length);
typedef void (*menu_print)(char *pData);

typedef struct MENU_TABLE MENU_TABLE;

struct MENU_TABLE 
{
   char *pCmd;
   char *pDescription;
   menu_callback mcb;
};

typedef struct
{
   menu_print pPrint;
}MENU;
   
#define M_NUM_CMDS(x) (sizeof(x) / sizeof(MENU_TABLE))
#define MAX_PRE_STRING 50                      

int initialize_menu(MENU *p_menu);
void print_menu(const MENU_TABLE *pCommands, int num_commands);
