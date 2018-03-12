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
#define TRUE 1
#define FALSE 0
char same = TRUE;
int i;
uint32 time;
uint16 Errors = 0;
uint16 NumISR = 0;
uint8 sourceArray[4096];
uint8 destinationArray[4096];
uint8 sourceISR = FALSE;
uint8 destinationISR = FALSE;

CY_ISR(TxISR) {
  sourceISR = TRUE;
  NumISR++;
  Pin_1_Write(1);
  /*
  while(1){
  LCD_Position(0,0);
  LCD_PrintString("AYE");
  }
  */
}
CY_ISR(RxISR) {
  destinationISR = TRUE;
  NumISR++;
  /*
  while(1){
  LCD_Position(0,0);
  LCD_PrintString("AYE2");
  }
  */
}

int main() {
  CyGlobalIntEnable; /* Enable global interrupts. */
  isr_1_StartEx(TxISR);
  isr_2_StartEx(RxISR);
  UART_Start();
  LCD_Start();
  LCD_Position(0, 0);
  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  for (i = 0; i < 4096; i++) {
    sourceArray[i] = i % 256;
  }
  for (i = 0; i < 4096; i++) {
    destinationArray[i] = 0;
  }
  for (i = 0; i < 4096; i++) {
    while (sourceISR == FALSE) {
    }
    if (sourceISR == TRUE) {
      sourceISR = FALSE;
      UART_PutChar(sourceArray[i]);
    }

    while (destinationISR == FALSE) {
    }
    if (destinationISR == TRUE) {
      destinationISR = FALSE;
      destinationArray[i] = UART_GetChar();
    }
  }

  // destinationArray[1] = 4;
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
    CyDelay(5000);
  }
  if (Errors > 0) {
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("Errors : ");
    LCD_Position(1, 0);
    LCD_PrintNumber(Errors);
    CyDelay(5000);
  }

  LCD_ClearDisplay();
  LCD_Position(0, 0);
  LCD_PrintString("Time:");
  LCD_PrintNumber(time);
  LCD_PrintString("us");
  CyDelay(5000);
  LCD_Position(1, 0);
  LCD_PrintString("Intrs:");
  LCD_PrintNumber(NumISR);
  for (;;) {
    /* Place your application code here. */
  }
}

/* [] END OF FILE */
