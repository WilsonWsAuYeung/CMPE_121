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

/* Included Libraries */
#include <project.h>

/* Main */
int main() {
  // initializes hardware LCD, PWM, and ADC
  LCD_Start();
  PWM_Start();
  ADC_Start();
  ADC_StartConvert();
  int32 output;
  // uint32 Uoutput;
  // uint16 Uoutput16;
  /* CyGlobalIntEnable;*/
  for (;;) {
    if (ADC_IsEndConversion(ADC_RETURN_STATUS)) {
      output = ADC_GetResult16();
      if (output < 0x0000) {
        output = 0;
      }
      if (output > 0xFDD1) {
        output = 0xFFFF;
      }

      // Uoutput=output;
      // Uoutput16=Uoutput;
      LCD_Position(0, 0);
      LCD_PrintString("Debug: ");
      LCD_PrintInt32(output);
      PWM_WriteCompare(output * 999 / 0xFFFF);
    }
  }
}

/* [] END OF FILE */
