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
#define Pin 1
int main() {
  // CyGlobalIntEnable; /* Enable global interrupts. */
  uint8 count = 0;
  /* Place your initialization/startup code here (e.g. MyInst_Start()) */

  for (;;) {
    // Method 1
    // per pin
    // CyPins_SetPin(LED_1_0);
    // CyDelay(1000);
    // CyPins_ClearPin(LED_1_0);
    // CyDelay(1000);

    // Method 2
    // component
    // LED_1_Write(!LED_1_Read());
    // CyDelay(1000);

    // Method 3
    // control register
    // Control_Reg_Write(count%2);
    // CyDelay(1000);
    // count++;
  }
}

/* [] END OF FILE */
