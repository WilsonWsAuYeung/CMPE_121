#include <project.h>
#include "FS.h"
#define FILENAME "game.txt"

FS_FILE *sd;

int main()
{
    /* Initialize the SD card */
    FS_Init();
    sd = FS_FOpen(FILENAME, "w+");
    FS_FWrite("hello", sizeof(char), 5, sd);
    FS_FClose(sd); 
}
/* [] END OF FILE */
