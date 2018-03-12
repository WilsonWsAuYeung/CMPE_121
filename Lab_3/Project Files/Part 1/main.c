/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#include <project.h>

#define REPEAT 0x45u
#define TRUE 1
#define FALSE 0
char same = TRUE;
int i;
uint32 time;
uint16 Errors = 0;
uint8 sourceArray[4096];
uint8 destinationArray[4096];

int main() {

  // Local variables

  // Start Interrupts
  // CyGlobalIntEnable; /* Enable global interrupts. */

  // Start components
  UART_Start();
  LCD_Start();
  LCD_Position(0, 0);

  for (i = 0; i < 4096; i++) {
    sourceArray[i] = i % 256;
  }

  for (i = 0; i < 4096; i++) {
    destinationArray[i] = 0;
  }
  uint8 FULL;
  // uint8 EMPTY;
  Timer_Start();

  for (i = 0; i < 4096; i++) {
    FULL = UART_ReadTxStatus();
    while (FULL == UART_TX_STS_FIFO_FULL) {
      FULL = UART_ReadTxStatus();
    }
    if (UART_TX_STS_FIFO_NOT_FULL) {
      UART_PutChar(sourceArray[i]);
    }

    while (~UART_ReadTxStatus() & UART_TX_STS_COMPLETE)
      ;
    {}

    destinationArray[i] = UART_GetChar();
  }
  Timer_Stop();
  time = (4294967296 - Timer_ReadCounter()) / 24;
  for (i = 0; i < 4096; i++) {
    if (sourceArray[i] != destinationArray[i]) {
      Errors++;
      same = FALSE;
    } else {
    }
  }

  if (Errors == 0) {
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("No Errors");
    CyDelay(200);
  }
  if (Errors > 0) {
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("Yes Errors : ");
    LCD_Position(1, 0);
    LCD_PrintNumber(Errors);
    CyDelay(5000);
  }

  LCD_ClearDisplay();
  LCD_Position(0, 0);
  LCD_PrintString("Time:");
  LCD_PrintInt32(time);
  LCD_PrintString("us");
  CyDelay(5000);

  for (;;) {
  }
}

/* [] END OF FILE */
