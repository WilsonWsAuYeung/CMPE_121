/* ========================================
 * Wilson Au-Yeung
 * wwauyeun
 * Lab 1
 * October 13, 2016
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
uint16 count = 0;
// char isr_flag = FALSE;
int32 PotVal;
int32 NewPWM;

int32 ExF;
int32 CxF;
int32 isr_count;
int32 new;
char isr_flag = FALSE;

CY_ISR(Interrupt) {
  isr_count = count;
  count = Counter_ReadCounter();
  new = count - isr_count;
  CxF = new * 500;
  isr_flag = TRUE;
  // Timer_STATUS;
  // Counter_STATUS;
  Timer_ReadStatusRegister();
}

/*
CY_ISR(Interrupt)
{
    count = Counter_ReadCounter();
    CxF = count*500;
    
    Counter_WriteCounter(0);
    isr_flag = TRUE;
    //Timer_STATUS;
    //Counter_STATUS;
    Timer_ReadStatusRegister();
}
*/
int main() {
  CyGlobalIntEnable; /* Enable global interrupts. */
  LCD_Start();
  ADC_Start();
  PWM_Start();
  ADC_StartConvert();
  Counter_Start();
  isr_StartEx(Interrupt);
  Timer_Start();

  LCD_Start();
  LCD_Position(0u, 0u);
  LCD_PrintString("P: ");

  for (;;) {
    if (ADC_IsEndConversion(ADC_RETURN_STATUS)) {
      PotVal = ADC_GetResult32();
      if (PotVal < 0x0000) {
        PotVal = 0x0000;
      }
      if (PotVal > 0xFDD1) {
        PotVal = 0xFFFF;
      }
      NewPWM = ((PotVal * 24000) / 0xFFFF);
      if (PotVal >= 2) {
        PWM_WritePeriod(NewPWM);
        PWM_WriteCompare(NewPWM / 2);
      } else {
        PWM_WritePeriod(2);
        PWM_WriteCompare(1);
      }
      LCD_Position(0u, 2u);
      LCD_PrintInt16(PotVal);
      ExF = (double)24000000 / (double)PWM_ReadPeriod();

      LCD_Position(1u, 0u);
      LCD_PrintInt32(ExF);
      if (isr_flag == TRUE) {
        isr_flag = FALSE;

        if (CxF > 0xB71b000) {
          CxF = 0xB71b000;
        }
        LCD_Position(0u, 7u);
        LCD_PrintInt32(CxF);
      }
    }
  }
}

/* [] END OF FILE */
