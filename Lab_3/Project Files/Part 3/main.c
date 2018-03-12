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
char flag = TRUE;
int i = 0;
int j = 0;
uint32 time;
uint16 Errors = 0;
uint8 sourceArray[4096];
uint8 destinationArray[4096];
uint8 sourceISR = FALSE;
uint8 destinationISR = FALSE;
int count = 0;
CY_ISR(TxISR) {

  while ((UART_ReadTxStatus() & UART_TX_STS_FIFO_NOT_FULL) && (i < 4096)) {
    UART_PutChar(sourceArray[i]);
    i++;
  }

  /*
   if(i < 4096){
               UART_PutChar(sourceArray[i]);
               i++;
   }
   */
}
CY_ISR(RxISR) {

  while ((UART_ReadRxStatus() & UART_RX_STS_FIFO_NOTEMPTY) && (j < 4096)) {
    destinationArray[j] = UART_GetChar();
    j++;
    // if(j == 4095){
    //     flag=FALSE;
    //}
  }
}

int main() {
  CyGlobalIntEnable; /* Enable global interrupts. */
  isr_1_StartEx(TxISR);
  isr_2_StartEx(RxISR);
  int k = 0;
  LCD_Start();
  LCD_Position(0, 0);
  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  for (count = 0; count < 4096; count++) {
    sourceArray[count] = count % 256;
  }
  for (count = 0; count < 4096; count++) {
    destinationArray[count] = 0;
  }

  i = 0;
  j = 0;
  UART_Start();
  Timer_Start();

  CyDelay(1000);
  LCD_ClearDisplay();
  LCD_Position(0, 0);
  LCD_PrintInt32(i);
  LCD_PrintString(" ");
  LCD_Position(1, 0);
  LCD_PrintInt32(j);
  CyDelay(5000);
  for (k = 0; k < 4096; k++) {
    if (sourceArray[k] != destinationArray[k]) {
      Errors++;
      same = FALSE;
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

  for (;;) {
    /* Place your application code here. */
  }
}

/* [] END OF FILE */
