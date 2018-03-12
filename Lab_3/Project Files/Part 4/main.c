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
char syncFlag = FALSE;
int i;
uint16 Errors = 0;
uint8 sourceArray[256];
uint8 destinationArray[256];
uint8 sourceISR = FALSE;
uint8 destinationISR = FALSE;
int32 speed = 0;
int32 NumTransmit = 0;
CY_ISR(TxISR) { sourceISR = TRUE; }
CY_ISR(RxISR) { destinationISR = TRUE; }
CY_ISR(TimerTxISR) {
  speed = NumTransmit * 11;

  NumTransmit = 0;
}
int main() {
  CyGlobalIntEnable; /* Enable global interrupts. */
  isr_1_StartEx(TxISR);
  isr_2_StartEx(RxISR);
  isr_3_StartEx(TimerTxISR);
  UART_Start();
  LCD_Start();
  LCD_Position(0, 0);
  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  for (i = 0; i < 256; i++) {
    sourceArray[i] = i % 256;
  }
  for (i = 0; i < 256; i++) {
    destinationArray[i] = 0;
  }
  i = 0;
  for (;;) {
    while (syncFlag == FALSE) {
      Pin_1_Write(1);
      while (sourceISR == FALSE) {
        // stall
      }
      if (sourceISR == TRUE) {
        sourceISR = FALSE;
        UART_PutChar(sourceArray[i]);
      }
      while (destinationISR == FALSE) {
        // stall
      }

      if (destinationISR == TRUE) {
        destinationISR = FALSE;
        destinationArray[i] = UART_GetChar();
        NumTransmit++;
      }
      if (destinationArray[i] != sourceArray[i]) {
        syncFlag = TRUE;
        i = (destinationArray[i] + 1) % 256;

      } else {
        syncFlag = TRUE;
      }

      LCD_ClearDisplay();
      LCD_Position(0, 0);
      LCD_PrintString("TX=");
      LCD_PrintNumber(i);
      LCD_PrintString(" RX=");
      LCD_PrintNumber(destinationArray[i]);
      LCD_Position(1, 0);
      if (destinationArray[i] != sourceArray[i])
        Errors++;
      // LCD_PrintString("Errors:");
      LCD_PrintNumber(Errors);
      LCD_PrintString("  ");
      LCD_PrintInt32(speed);
      i++;
      // CyDelay(1000);
    }
    if (syncFlag == TRUE) {

      for (i = i; i < 256; i++) {
        while (sourceISR == FALSE) {
        }
        if (sourceISR == TRUE) {
          sourceISR = FALSE;
          UART_PutChar(sourceArray[i]);
          NumTransmit++;
        }

        while (destinationISR == FALSE) {
        }
        if (destinationISR == TRUE) {
          destinationISR = FALSE;
          destinationArray[i] = UART_GetChar();
        }
        LCD_ClearDisplay();
        LCD_Position(0, 0);
        LCD_PrintString("TX=");
        LCD_PrintNumber(i);
        LCD_PrintString(" RX=");
        LCD_PrintNumber(destinationArray[i]);
        LCD_Position(1, 0);
        if (destinationArray[i] != sourceArray[i])
          Errors++;
        // LCD_PrintString("Errors:");
        LCD_PrintNumber(Errors);
        LCD_PrintString("  ");
        LCD_PrintInt32(speed);
        CyDelay(50);
      }
      i = 0;
    }
  }
}

/* [] END OF FILE */
